#!/usr/bin/env python3
"""Maintain the checked-in C port; ordinary builds never invoke this tool."""
import argparse
import hashlib
import json
from pathlib import Path
import shutil
import subprocess
import tempfile


def digest(data):
    return hashlib.sha256(data).hexdigest()


def refresh_manifest(source, port, baseline):
    """Accept an explicit new pristine baseline after the port patch is rebased."""
    baseline = subprocess.check_output(
        ['git', 'rev-parse', baseline + '^{commit}'], cwd=source, text=True).strip()
    object_format = subprocess.check_output(
        ['git', 'rev-parse', '--show-object-format'], cwd=source, text=True).strip()
    listing = subprocess.check_output(
        ['git', 'ls-tree', '-r', '-z', baseline, '--', 'src', 'util'], cwd=source)
    upstream, modes = {}, {}
    for entry in listing.split(b'\0'):
        if not entry:
            continue
        metadata, name = entry.split(b'\t', 1)
        mode, kind, blob = metadata.decode().split()
        name = name.decode()
        if kind != 'blob' or mode not in ('100644', '100755'):
            raise SystemExit('Unsupported upstream source entry: ' + name)
        data = (source / name).read_bytes()
        value = hashlib.new(object_format)
        value.update(('blob %d\0' % len(data)).encode())
        value.update(data)
        if value.hexdigest() != blob:
            raise SystemExit('Source differs from requested pristine baseline: ' + name)
        upstream[name] = digest(data)
        modes[name] = int(mode, 8) & 0o777
    actual = {str(p.relative_to(source)) for tree in ('src', 'util')
              for p in (source / tree).rglob('*') if p.is_file()}
    if actual != set(upstream):
        raise SystemExit('Source inventory differs from requested pristine baseline')
    with tempfile.TemporaryDirectory(prefix='edg-c-port-upgrade-') as temporary:
        staging = Path(temporary)
        for name in upstream:
            target = staging / name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(source / name, target)
            target.chmod(modes[name])
        command = ['git', 'apply', '--binary', '--whitespace=nowarn']
        subprocess.run(command + ['--check', str(port / 'c-port.patch')], cwd=staging, check=True)
        subprocess.run(command + [str(port / 'c-port.patch')], cwd=staging, check=True)
        patched = {str(p.relative_to(staging)): digest(p.read_bytes())
                   for p in sorted(staging.rglob('*')) if p.is_file()}
        derived = {name: checksum for name, checksum in patched.items()
                   if Path(name).suffix in ('.c', '.h') or
                   name in ('src/error_msg.txt', 'src/error_tag.txt',
                            'util/make_predef_macro_table')}
        manifest = {'baseline': baseline, 'upstream_files': upstream,
                    'upstream_modes': modes, 'patched_files': patched,
                    'derived_files': derived,
                    'derived_modes': {name: (staging / name).stat().st_mode & 0o777
                                      for name in derived},
                    'patch_sha256': digest((port / 'c-port.patch').read_bytes())}
    temporary = port / 'manifest.json.tmp'
    temporary.write_text(json.dumps(manifest, sort_keys=True, indent=2) + '\n')
    temporary.replace(port / 'manifest.json')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--source', type=Path, help='repository containing pristine src/ and util/')
    parser.add_argument('--output', type=Path, help='edg2qbe directory to regenerate')
    parser.add_argument('--check', action='store_true', help='compare without writing derived files')
    parser.add_argument('--refresh-manifest', action='store_true',
                        help='record an explicitly selected pristine baseline and rebased patch')
    parser.add_argument('--baseline', help='git commit/tag required with --refresh-manifest')
    args = parser.parse_args()
    port = Path(__file__).resolve().parent
    source = (args.source or port.parent.parent).resolve()
    output = (args.output or port.parent).resolve()
    if args.refresh_manifest:
        if not args.baseline or args.check:
            parser.error('--refresh-manifest requires --baseline and cannot be combined with --check')
        refresh_manifest(source, port, args.baseline)
    elif args.baseline:
        parser.error('--baseline requires --refresh-manifest')
    manifest = json.loads((port / 'manifest.json').read_text())
    patch = (port / 'c-port.patch').read_bytes()
    if digest(patch) != manifest['patch_sha256']:
        raise SystemExit('C-port patch digest mismatch')
    expected = set(manifest['upstream_files'])
    actual = {str(p.relative_to(source)) for name in ('src', 'util')
              for p in (source / name).rglob('*') if p.is_file()}
    if actual != expected:
        raise SystemExit('Pristine input file inventory mismatch: missing=%s extra=%s' %
                         (sorted(expected - actual), sorted(actual - expected)))
    with tempfile.TemporaryDirectory(prefix='edg-c-port-') as temporary:
        staging = Path(temporary)
        for name, checksum in manifest['upstream_files'].items():
            data = (source / name).read_bytes()
            if digest(data) != checksum:
                raise SystemExit('Pristine input digest mismatch: ' + name)
            target = staging / name
            target.parent.mkdir(parents=True, exist_ok=True)
            target.write_bytes(data)
            target.chmod(manifest['upstream_modes'][name])
        # Exact input digests prevent offset/fuzz upgrades or an unnoticed new baseline.
        command = ['git', 'apply', '--binary', '--whitespace=nowarn']
        subprocess.run(command + ['--check', str(port / 'c-port.patch')], cwd=staging, check=True)
        subprocess.run(command + [str(port / 'c-port.patch')], cwd=staging, check=True)
        produced = {str(p.relative_to(staging)) for p in staging.rglob('*') if p.is_file()}
        if produced != set(manifest['patched_files']):
            raise SystemExit('Patched output inventory mismatch')
        for name, checksum in manifest['patched_files'].items():
            if digest((staging / name).read_bytes()) != checksum:
                raise SystemExit('Patched output digest mismatch: ' + name)
        # Discard pristine inputs that are not part of the published C snapshot.
        for name in produced - set(manifest['derived_files']):
            (staging / name).unlink()
        for directory in sorted(staging.rglob('*'), reverse=True):
            if directory.is_dir() and not any(directory.iterdir()):
                directory.rmdir()
        for tree, destination in (('src', 'edg'), ('util', 'util')):
            target = output / destination
            if args.check:
                wanted = {str(p.relative_to(staging / tree)): p.read_bytes()
                          for p in (staging / tree).rglob('*') if p.is_file()}
                found = {str(p.relative_to(target)): p.read_bytes()
                         for p in target.rglob('*') if p.is_file()}
                if found != wanted:
                    raise SystemExit('Checked-in derived tree mismatch: ' + str(target))
                for name, mode in manifest['derived_modes'].items():
                    if name.startswith(tree + '/') and (target / name[len(tree) + 1:]).stat().st_mode & 0o777 != mode:
                        raise SystemExit('Checked-in derived mode mismatch: ' + name)
            else:
                target.mkdir(parents=True, exist_ok=True)
                for child in target.iterdir():
                    if child.is_dir():
                        shutil.rmtree(child)
                    else:
                        child.unlink()
                shutil.copytree(staging / tree, target, dirs_exist_ok=True)
    print(('Verified' if args.check else 'Regenerated') + ' EDG C port from ' + manifest['baseline'])


if __name__ == '__main__':
    main()
