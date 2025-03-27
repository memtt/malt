#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.4
#    DATE     : 10/2024
#    LICENSE  : CeCILL-C
#    FILE     : dev/build_and_test_on_distributions.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2024
#    AUTHOR   : Sébastien Valat - 2024
############################################################

############################################################
import os
import argparse
from gen_archive import malt_dev_gen_archive

############################################################
def get_malt_source_dir() -> str:
    return os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

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
    malt_dev_gen_archive()

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
    archive.set_defaults(func=command_archive)

    # parse
    args = parser.parse_args()

    # apply sub command
    args.func(args)

############################################################
if __name__ == "__main__":
    main()
