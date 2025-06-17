#!/usr/bin/env python3
############################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.3.0
#    DATE     : 03/2025
#    LICENSE  : CeCILL-C
#    FILE     : dev/update_file_headers.py
#-----------------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2024
#    AUTHOR   : Sébastien Valat (INRIA) - 2025
############################################################

# Usage : 
# --------------------------------------------------------------------------------------------------
# python3 -m venv venv
# source venv/bin/activate
# pip install gitpython
# ./dev/update_file_headers.py --git-all .
# --------------------------------------------------------------------------------------------------

############################################################
# python
import re
import os
import json
import fnmatch
import datetime
import argparse
import subprocess
# git
from git import Repo

############################################################
class HeaderPatcher:
    def __init__(self, config_file: str) -> None:
        # vars
        self.config_file = config_file
        self.config = {}

        # load
        self.load_config(config_file)

    def load_config(self, config_file: str) -> None:
        with open(config_file, "r") as fp:
            config = json.load(fp)
            for id, entry in enumerate(config['exclude_summary_regexps']):
                config['exclude_summary_regexps'][id] = re.compile(entry)
            self.config = config

    def git_load_blame_log(self, file: str) -> list:
        # open git repo
        repo = Repo("./")
        assert not repo.bare
        
        # blame
        blame = repo.git.blame('HEAD', file, '--porcelain').split('\n')

        # create log
        log = []

        # set default vars to track
        commit_hash = ''
        commiter_mail = 'INIT'
        commiter_name = 'INIT'
        commiter_time = 'INIT'
        commiter_zone = 'INIT'
        summary = 'INIT'

        # loop on all entries to extract the infos
        for entry in blame:
            parts = entry.split(' ', maxsplit=1)
            key = parts[0]
            if len(key) == 40:
                commit_hash = key
            if key == 'summary':
                summary = parts[1]
            elif key == 'author-mail':
                commiter_mail = parts[1]
            if key == 'author':
                commiter_name = parts[1]
            elif key == 'author-time':
                commiter_time = parts[1]
            elif key == 'filename':
                commiter_zone = parts[1]
                log.append({'mail': commiter_mail, 'name': commiter_name, 'time': commiter_time, 'zone': commiter_zone, 'hash': commit_hash, 'summary': summary})
                commit_hash = ''
                commiter_mail = 'INIT'
                commiter_name = 'INIT'
                commiter_time = 'INIT'
                commiter_zone = 'INIT'
                summary = 'INIT'

        #ok
        return log

    def get_mail_affiliation(self, mail: str) -> (str|None):
        domain = '@' + mail[1:-1].split('@')[1]
        affiliation_domains = self.config['affiliation_domains']
        if domain in affiliation_domains:
            return affiliation_domains[domain]
        elif mail in affiliation_domains:
            return affiliation_domains[domain]
        else:
            None

    def fix_name(self, name: str) -> str:
        authors_name_fixes = self.config['authors_name_fixes']
        if name in authors_name_fixes:
            return authors_name_fixes[name]
        else:
            return name

    def need_skip_blame_entry(self, blame_entry: dict) -> bool:
        exclude_hashes = self.config['exclude_hashes']
        # skip some based on hash
        skip = False
        if blame_entry['hash'] in exclude_hashes:
            skip = True
        # skip based on summary (not to get commit which push file headers update)        
        for regexp in self.config['exclude_summary_regexps']:
            if regexp.match(blame_entry['summary']):
                skip = True
        return skip

    def git_extract_authors_from_history(self, blame_log: list) -> dict:
        # build final storage
        authors = {}
        exclude_hashes = self.config['exclude_hashes']

        # loop on all entries
        for entry in blame_log:
            # apply skip
            if self.need_skip_blame_entry(entry):
                continue
            
            # build summary
            author = entry['mail']
            name = self.fix_name(entry['name'])
            affiliation = self.get_mail_affiliation(entry['mail'])
            if affiliation:
                full_name = f"{name} ({affiliation})"
            else:
                full_name = name
            date = datetime.datetime.fromtimestamp(int(entry['time']))
            year = date.year
            month = date.month
            if not full_name in authors:
                authors[full_name] = {
                    'full-name': full_name,
                    'author': author,
                    'year-start': year,
                    'year-end': year,
                    'last-month': month,
                }

            # set entry
            auth_entry = authors[full_name]
            auth_entry['year-start'] = min(auth_entry['year-start'], year)
            auth_entry['year-end'] = max(auth_entry['year-end'], year)

        # ok
        return authors

    def get_last_edit_month_year(self, blame_log: list) -> str:
        # vars
        last_month = 0
        last_year = 0
        
        # loop on all entries
        for entry in blame_log:
            # apply skip
            if self.need_skip_blame_entry(entry):
                continue

            date = datetime.datetime.fromtimestamp(int(entry['time']))
            year = date.year
            month = date.month
            if year > last_year:
                last_year = year
                last_month = month
            elif year == last_year and month > last_month:
                last_month = month

        # ok
        return f"{last_month:02d}/{last_year:04d}"

    def build_infos(self, filename: str) -> dict:
        # build default
        infos = {
            'header': self.config['header_keys'].copy(),
            'origin': None,
            'width': self.config['width']
        }
        
        # set file
        infos['header']['FILE'] = filename
        
        # apply specific for some files
        if filename in self.config['specific']:
            specific = self.config['specific'][filename]
            # has external origin
            if 'origin' in specific:
                infos['origin'] = specific['origin']
            # override value (license....)
            if 'override' in specific:
                for key, value in specific['override'].items():
                    infos['header'][key] = value
            if 'width' in specific:
                infos['width'] = specific['width']

        # ok
        return infos

    def build_new_file_header(self, authors_list: list, last_edit: str, infos: dict, lang: dict) -> list:   
        # pattern
        info_open = lang['info_open']
        open_start = lang['open_start']
        open_end = lang['open_end']
        close_start = lang['close_start']
        close_end = lang['close_end']
        width = infos['width']
        line_start = open_start + lang['repeat']*(width - len(open_start) - len(open_end)) + open_end
        line_end = close_start + lang['repeat']*(width - len(close_start) - len(close_end)) + close_end
        
        # build new header
        header_script = []
        width = infos['width']
        header_script.append( f'{line_start}\n')
        for key, value in infos['header'].items():
            fixed_value = value.replace('@LAST_EDIT_MONTH_YEAR@', last_edit)
            header_script.append(f'{info_open}    {key: <8} : {fixed_value}\n')
        header_script.append( f'{info_open}{"-"*(width-len(info_open))}\n')

        # add authors
        for author in authors_list:
            full_name = author['full-name']
            start = author['year-start']
            end = author['year-end']
            if start == end:
                years = str(start)
            else:
                years = f"{start} - {end}"
            header_script.append(f'{info_open}    AUTHOR   : {full_name} - {years}\n')
            
        # origin
        if infos['origin']:
            header_script.append( info_open + " ORIGIN ".center(width-1, '-')+'\n')
            for key, value in infos['origin'].items():
                header_script.append(f'{info_open}    {key: <8} : {value}\n')

        # close
        header_script.append( f'{line_end}\n')

        # ok
        return header_script

    def calc_lang(self, filename: str) -> dict:
        # get it
        lang = self.config['lang']
        fname = os.path.basename(filename)

        # apply filters
        for name, params in lang.items():
            apply_on = params['apply_on']
            for selector in apply_on:
                if fnmatch.fnmatch(fname, selector):
                    print(f" - {filename} : apply type {name}")
                    return params

        # not found
        raise Exception(f"Invalid file type : {filename}")

    def patch_header_lines(self, origin_content: list, new_header: list, width: int, lang: dict) -> list:
        # build patched
        patched_content = None
        
        # pattern
        info_open = lang['info_open']
        open_start = lang['open_start']
        open_end = lang['open_end']
        close_start = lang['close_start']
        close_end = lang['close_end']
        line_start = open_start + lang['repeat']*(width - len(open_start) - len(open_end)) + open_end
        line_end = close_start + lang['repeat']*(width - len(close_start) - len(close_end)) + close_end

        # loop on content to find the start of header
        for line, value in enumerate(origin_content):
            if value == f'{line_start}\n' and (f'{info_open}    PROJECT  : ' in origin_content[line + 1] or '#            PROJECT  : MA' in origin_content[line + 1]):
                count = 1
                while origin_content[line + count] != f'{line_end}\n':
                    count += 1
                del origin_content[line:line+count+1]
                patched_content = origin_content[0:line] + new_header + origin_content[line:]
                break
            elif line > 10:
                break

        # add on top if not present
        if len(origin_content) == 0:
            patched_content = origin_content
        elif not patched_content and origin_content[0][0:2] == '#!':
            patched_content = origin_content[0:1] + new_header + origin_content[1:]
        elif not patched_content:
            patched_content = new_header + origin_content

        # ok
        return patched_content

    def order_authors_by_date(self, authors: dict) -> list:
        per_year = {}
        for key, author in authors.items():
            year = author['year-start']
            if not year in per_year:
                per_year[year] = []
            per_year[year].append(author)
            
        result = []
        for year in sorted(per_year.keys()):
            auths = per_year[year]
            for author in auths:
                result.append(author)

        return result

    def patch_header(self, file: str) -> None:
        # extract log
        blame_log = self.git_load_blame_log(file)

        # extract authors
        authors = self.git_extract_authors_from_history(blame_log)
        last_edit = self.get_last_edit_month_year(blame_log)
        
        # as list
        authors_list = self.order_authors_by_date(authors)
        
        # build infos
        infos = self.build_infos(file)
        
        # get lang
        lang = self.calc_lang(file)

        # build header as list of lines
        header_script = self.build_new_file_header(authors_list, last_edit, infos, lang)

        # load file
        with open(file, 'r') as fp:
            original_content = fp.readlines()

        # patch
        patched_content = self.patch_header_lines(original_content, header_script, infos['width'], lang)

        # save again
        with open(file, 'w+') as fp:
            content = fp.writelines(patched_content)

    def is_in_include_files(self, filename: str) -> bool:
        # check if included
        include = self.config['include_files']
        for ipattern in include:
            if fnmatch.fnmatch(filename, ipattern) or filename.startswith(ipattern):
                return True
        return False

    def patch_file(self, filename: str):
        # if exclude
        exclude = self.config['exclude_files']
        for pattern in exclude:
            if fnmatch.fnmatch(filename, pattern) or filename.startswith(pattern):
                if not self.is_in_include_files(filename):
                    print(f" - {filename} : EXCLUDED")
                    return
        
        # apply
        self.patch_header(filename)

    def patch_all_git_files(self, git_dir: str) -> None:
        files = subprocess.getoutput("git ls-files").split('\n')
        for file in files:
            self.patch_file(file)

############################################################
def run_from_args(args):
    # load
    patcher = HeaderPatcher(args.config)

    # patch
    if args.git_all:
        patcher.patch_all_git_files(args.FILE)
    else:
        patcher.patch_file(args.FILE)

############################################################
def config_arg_parser(parser: argparse.ArgumentParser):
    # calc default config path
    default_config = os.path.join(os.path.dirname(__file__), 'update_file_headers.json')

    # add options
    parser.add_argument('FILE', help='File to patch or source dir when using --git-all')
    parser.add_argument('--git-all', '-g', help="Run on all file from GIT. FILE in this case give the GIT dir.", action="store_true")
    parser.add_argument('--config', '-c', help="Config file to use", default=default_config)

############################################################
def main():
    # define arguments
    parser = argparse.ArgumentParser()
    config_arg_parser(parser)

    # parse
    options = parser.parse_args()
    
    # run
    run_from_args(options)

############################################################
if __name__ == "__main__":
    main()
