#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 10/2025
#    LICENSE  : CeCILL-C
#    FILE     : dev/dev.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
import os
import sys
import argparse
from gen_archive import malt_dev_gen_archive, HashMode
from gen_coverage import malt_gen_coverage
from update_file_headers import config_arg_parser, run_from_args
from update_version import malt_update_version

############################################################
def get_malt_source_dir() -> str:
    return os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

############################################################
def get_malt_version() -> str:
    version_file = os.path.join(get_malt_source_dir(), ".version")
    with open(version_file, 'r') as fp:
        return fp.read().replace('\n', '')

############################################################
def command_portability(args: object) -> None:
    # get source path
    source_dir = get_malt_source_dir()

    # build options
    lst = []
    if args.verbose:
        lst += ["-v"]
    lst += args.options[1:]
    opt_str = " ".join(lst)

    # run from that
    os.chdir(source_dir)
    os.system(f"./dev/build_and_test_on_distributions.py {opt_str}")

############################################################
def command_archive(args: object) -> None:
    os.chdir(get_malt_source_dir())
    if args.hash and args.no_hash:
        print("Error: cannot use --hash and --no-hash at same time !")
        sys.exit(1)
    elif args.hash:
        hash_mode = HashMode.YES
    elif args.no_hash:
        hash_mode = HashMode.NO
    else:
        hash_mode = HashMode.AUTO
    malt_dev_gen_archive(version = args.version, hash_mode=hash_mode, commit=args.commit)

############################################################
def command_coverage(args: object) -> None:
    malt_gen_coverage()

############################################################
def command_update_file_headers(args: object) -> None:
    run_from_args(args)

############################################################
def command_version(args) -> None:
    extra = ""
    if args.beta:
        extra = "-beta"
    elif args.dev:
        extra = "-dev"
    malt_update_version(args.version, extra)

############################################################
def main() -> None:
    # parse args
    parser = argparse.ArgumentParser("dev", description="Globale source manager of the MALT repo.")
    subparser = parser.add_subparsers(required=True, title='command', dest='command', help="Command to apply")

    # sub command
    portability = subparser.add_parser('portability', aliases=['port'], help="Run the MALT build & tests in varios Podman images.")
    portability.add_argument("--verbose", "-v", help="Make verbose.", action="store_true")
    portability.add_argument("options", help="Options to trasnfer to the pytest command.", nargs=argparse.REMAINDER)
    portability.set_defaults(func=command_portability)

    # sub command
    archive = subparser.add_parser('archive', aliases=['ar'], help="Generate delivery archive.")
    archive.add_argument("--version", "-v", help="Version to generate, by default the value in .version file.", default=get_malt_version())
    archive.add_argument("--hash", help="Add the git hash to the archive version name (by default only if not on a release commit).", action="store_true")
    archive.add_argument("--no-hash", help="Skip the git hash to the archive version.", action="store_true")
    archive.add_argument("--commit", "-c", help="Commit to use to generate the archive.", default="HEAD")
    archive.set_defaults(func=command_archive)

    # sub command
    coverage = subparser.add_parser('coverage', aliases=['cov'], help="Generate coverage in current build directory.")
    coverage.set_defaults(func=command_coverage)

    # sub command
    update_file_headers = subparser.add_parser('headers', aliases=['head'], help="UPdate file headers.")
    config_arg_parser(update_file_headers)
    update_file_headers.set_defaults(func=command_update_file_headers)
    
    # sub command
    update_version = subparser.add_parser('version', aliases=['ver'], help="Update version in sources")
    update_version.add_argument('version', help="Set the version number")
    update_version.add_argument('--beta', help="Mark the version as beta", action="store_true")
    update_version.add_argument('--dev', help="Mark the version as dev branch", action="store_true")
    update_version.set_defaults(func=command_version)

    # parse
    args = parser.parse_args()

    # apply sub command
    args.func(args)

############################################################
if __name__ == "__main__":
    main()
