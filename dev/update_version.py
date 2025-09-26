#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    DATE     : 09/2025
#    LICENSE  : CeCILL-C
#    FILE     : dev/update_version.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

############################################################
import os
import subprocess
from update_file_headers import HeaderPatcher
from termcolor import cprint

############################################################
class UpdateVersion:
    def __init__(self):
         # set defaults
         self.old_version = "X.X.X-dev"
         self.old_short_version = "X.X.X"
         self.old_extra = "-dev"

         # check
         self.check_is_source_root_dir()

         # load
         self.extract_old_version()

    def extract_old_version(self):
        with open("./.version", "r") as fp:
            self.old_version = fp.readline().replace("\n", "")
        self.old_short_version = self.old_version.replace("-dev", "").replace("-beta", "").replace("\n", "")
        self.old_extra = self.old_version.replace(self.old_short_version, "")
        print(f"[version] Old version is {self.old_version}, short is {self.old_short_version}")

    def check_is_source_root_dir(self):
        print("[check] Source dir")
        if not os.path.exists("./dev/dev.py"):
            raise Exception("Caution, you must call this script from the project root directory !")

    def rebuild_manpages(self):
        print("[manpages] Rebuild ...")
        subprocess.run("make", cwd="./src/manpages")
        print("[manpages] Rebuild done")

    def update_file_headers_version_and_authors(self, target_version):
        print("[headers] Update version & authors in file headers ...")
        print("------------------------------------------------------")
        
         # load
        patcher = HeaderPatcher("./dev/update_file_headers.json")
        patcher.patch_all_git_files(".")

        print("------------------------------------------------------")
        print("[headers] Update version & authors in file headers done")

    def line_contains(self, line: str, what: list) -> bool:
        for entry in what:
            if not entry in line:
                return False
        return True

    def patch_specific_file(self, file:str, search:list, old_version: str, target_version:str):
        print(f"[patch] Patch {file}")
        # load
        with open(file, 'r') as fp:
            lines = fp.readlines()

        # patch
        for id, line in enumerate(lines):
            if self.line_contains(line, search):
                lines[id] = line.replace(old_version, target_version)

        # dump
        with open(file, "w+") as fp:
            fp.writelines(lines)

    def patch_specific_files(self, target_version:str, extra:str):
        print("[patch] Patch specific files ...")
        self.patch_specific_file("./dev/update_file_headers.json", ['VERSION'], self.old_version, target_version + extra)
        self.patch_specific_file("./CMakeLists.txt", ['project(MALT VERSION'], self.old_short_version, target_version)
        self.patch_specific_file("./CMakeLists.txt", ['set(MALT_VERSION'], self.old_short_version, target_version)
        self.patch_specific_file("./CMakeLists.txt", ['set(MALT_VERSION_NOTE'], f'set(MALT_VERSION_NOTE "{self.old_extra}")', f'set(MALT_VERSION_NOTE "{extra}")')
        self.patch_specific_file("./Doxyfile", ['PROJECT_NUMBER', '='], self.old_version, target_version + extra)
        self.patch_specific_file("./.vscode/snippets/cpp.json", ['VERSION',':'], self.old_version, target_version + extra)
        self.patch_specific_file("./.vscode/snippets/css.json", ['VERSION',':'], self.old_version, target_version + extra)
        self.patch_specific_file("./.vscode/snippets/shellscript.json", ['VERSION',':'], self.old_version, target_version + extra)
        self.patch_specific_file("./dev/gen_archive.py", ['PACKAGE_VERSION='], self.old_version, target_version+extra)
        self.patch_specific_file("./dev/packaging.sh", ['VERSION='], self.old_version, target_version+extra)
        self.patch_specific_file("src/webview/client-files/package.json", ['VERSION', ":"], self.old_version, target_version+extra)
        self.patch_specific_file("src/webview/package.json", ['VERSION', ":"], self.old_version, target_version+extra)
        self.patch_specific_file("src/manpages/malt.ronn", ['MALT version', "manpage of"], self.old_version, target_version + extra)
        self.patch_specific_file("src/manpages/malt-webview.ronn", ['MALT version', "manpage of"], self.old_version, target_version + extra)
        self.patch_specific_file("src/manpages/malt-passwd.ronn", ['MALT version', "manpage of"], self.old_version, target_version + extra)
        self.patch_specific_file("src/libinstrum/tests/TestValgrindOutput.cpp", ['creator:', "MALT-"], self.old_version, target_version + extra)
        self.patch_specific_file("src/libinstrum/core/tests/TestSimpleCallStackTracker.cpp", ['creator:', "MALT-"], self.old_version, target_version + extra)
        self.patch_specific_file("packaging/archlinux/PKGBUILD", ['pkgver='], self.old_version, target_version + extra)
        self.patch_specific_file("packaging/fedora/malt.spec", ['Version: '], self.old_version, target_version + extra)
        self.patch_specific_file("packaging/README.md", [], self.old_short_version, target_version)
        self.patch_specific_file("packaging/debian/changelog", ['Version', 'of malt'], self.old_short_version, target_version)
        self.patch_specific_file("packaging/debian/changelog", ['malt', 'UNRELEASED'], self.old_short_version, target_version)
        self.patch_specific_file("src/reader/libreader/extractors/tests/example.json", [], self.old_version, target_version + extra)
        self.patch_specific_file("src/reader/libreader/extractors/tests/example.expected.json", [], self.old_version, target_version + extra)
        self.patch_specific_file("src/doc/file-format.md", [], self.old_short_version, target_version)
        print("[patch] Patch specific files done")

    def rename_some_files_with_version(self, target_version, extra):
        old=self.old_version.replace("-beta", "_beta").replace("-dev", "_dev")
        fixed_extra=extra.replace("-", "_")
        subprocess.run(['git','mv',f'packaging/gentoo/dev-util/malt/malt-{old}.ebuild', f'packaging/gentoo/dev-util/malt/malt-{target_version}{fixed_extra}.ebuild'])
        self.patch_specific_file(f'packaging/gentoo/dev-util/malt/malt-{target_version}{fixed_extra}.ebuild', ['FILE'], self.old_version, target_version + extra)
        self.patch_specific_file(f"packaging/gentoo/dev-util/malt/malt-{target_version}{fixed_extra}.ebuild", ['tar.bz2', 'SRC_URI'], self.old_version, target_version + extra)

    def save_version(self, target_version, extra):
        with open("./.version", "w+") as fp:
            fp.write(f"{target_version}{extra}\n")

############################################################
def malt_update_version(target_version, extra):
    print(f"[version] Set version {target_version}{extra}")
    updater = UpdateVersion()
    updater.check_is_source_root_dir()
    updater.extract_old_version()
    updater.patch_specific_files(target_version, extra)
    updater.update_file_headers_version_and_authors(target_version + extra)
    updater.rebuild_manpages()
    updater.rename_some_files_with_version(target_version, extra)
    updater.save_version(target_version, extra)
