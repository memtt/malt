#!/usr/bin/env python3
######################################################
#    PROJECT  : MALT (MALoc Tracker)
#    VERSION  : 1.2.2
#    DATE     : 04/2024
#    LICENSE  : CeCILL-C
#-----------------------------------------------------
#    AUTHOR   : Sébastien Valat - 2024
######################################################

# Usage : 
# --------------------------------------------------------------------------------------------------
# for file in $(git ls-files); do ./dev/update_file_headers.py $file; done
# --------------------------------------------------------------------------------------------------

######################################################
# python
import re
import os
import fnmatch
import datetime
import argparse
# git
from git import Repo

######################################################
LANG_SHELL = {
    'open_start': '',
    'open_end': '',
    'repeat': '#',
    'info_open': '#',
    'close_start': '',
    'close_end': ''
}
LANG_C = {
    'open_start': '/*',
    'open_end': '',
    'repeat': '*',
    'info_open': '*',
    'close_start': '',
    'close_end': '*/'
}

######################################################
config = {
    "header_keys": {
        "PROJECT": "MALT (MALoc Tracker)",
        "VERSION": "1.2.2",
        "DATE": "@LAST_EDIT_MONTH_YEAR@",
        "LICENSE": "CeCILL-C"
    },
    "width": 60,
    "specific": {
        "configure": {
            "origin": {
                'PROJECT': 'numaprof',
                'VERSION': '1.1.2',
                'DATE': '01/2022',
                'AUTHOR': 'Valat Sébastien (CERN) - 2017 - 2018',
                'LICENSE': 'Apache 2.0',
                'URL': 'https://memtt.github.io/numaprof/'
            },
            "override": {
                "LICENSE": "Apache 2.0"
            },
            "width": 59
        }
    },
    "authors_name_fixes": {
        "dependabot[bot]": None,
        "sebv":            "Sébastien Valat",
        "Sebastien Valat": "Sébastien Valat",
        "Valat Sébastien": "Sébastien Valat"
    },
    "affiliation_domains": {
        '@atos.net': "ATOS",
        '@cern.ch': "CERN",
        '@inria.fr': "INRIA",
        '@exascale-computing.eu': 'ECR',
        '@gentoo.org': 'Gentoo',
        '@rc.gc.ca': 'ECCC',
        '@ddn.com': 'DDN',
        '@lanl.gov': 'LANL',
        '@parsteur.fr': 'Institut Pasteur'
    },
    "exclude_hashes": [
        #'940423b7275caccb487a214acfe7abbd71b97aec',
    ],
    "exclude_summary_regexps": [
        re.compile(r"^\[version\]"),
    ],
    "lang": {
        "shell": {
            'open_start': '',
            'open_end': '',
            'repeat': '#',
            'info_open': '#',
            'close_start': '',
            'close_end': '',
            'apply_on': [
                '*.py',
                '*.sh',
                "CMakeLists.txt",
                "options",
                "configure"
            ]
        },
        "c": {
            'open_start': '/*',
            'open_end': '',
            'repeat': '*',
            'info_open': '*',
            'close_start': '',
            'close_end': '*/',
            'apply_on': [
                '*.c',
                '*.cpp',
                '*.cxx',
                '*.h',
                '*.hpp',
                '*.hxx',
            ]
        }
    },
    "exclude_files": [
        "extern-deps/from-fftw",
        "extern-deps/from-htopml",
        "extern-deps/googletest-1.14.0",
        "extern-deps/iniparser",
    ]
}

######################################################
def git_load_blame_log(file: str) -> list:
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
        elif key == 'author-tz':
            commiter_zone = parts[1]
            log.append({'mail': commiter_mail, 'name': commiter_name, 'time': commiter_time, 'zone': commiter_zone, 'hash': commit_hash, 'summary': summary})

    #ok
    return log

######################################################
def get_mail_affiliation(mail: str) -> (str|None):
    domain = '@' + mail[1:-1].split('@')[1]
    affiliation_domains = config['affiliation_domains']
    if domain in affiliation_domains:
        return affiliation_domains[domain]
    else:
        None

######################################################
def fix_name(name: str) -> str:
    authors_name_fixes = config['authors_name_fixes']
    if name in authors_name_fixes:
        return authors_name_fixes[name]
    else:
        return name

######################################################
def git_extract_authors_from_history(blame_log: list) -> dict:
    # build final storage
    authors = {}
    exclude_hashes = config['exclude_hashes']

    # loop on all entries
    for entry in blame_log:
        # skip some based on hash
        skip = False
        if entry['hash'] in exclude_hashes:
            skip = True
        # skip based on summary (not to get commit which push file headers update)        
        for regexp in config['exclude_summary_regexps']:
            if regexp.match(entry['summary']):
                skip = True

        # apply skip
        if skip:
            continue
        
        # build summary
        author = entry['mail']
        name = fix_name(entry['name'])
        affiliation = get_mail_affiliation(entry['mail'])
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

######################################################
def get_last_edit_month_year(blame_log: list) -> str:
    # vars
    last_month = 0
    last_year = 0
    
    # loop on all entries
    for entry in blame_log:
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

######################################################
def build_infos(filename: str) -> dict:
    # build default
    infos = {
        'header': config['header_keys'].copy(),
        'origin': None,
        'width': config['width']
    }
    
    # set file
    infos['header']['FILE'] = filename
    
    # apply specific for some files
    if filename in config['specific']:
        specific = config['specific'][filename]
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

######################################################
def build_new_file_header(authors_list: list, last_edit: str, infos: dict, lang: dict) -> list:   
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

######################################################
def calc_lang(filename: str) -> dict:
    # get it
    lang = config['lang']
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

######################################################
def patch_header_lines(origin_content: list, new_header: list, width: int, lang: dict) -> list:
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

######################################################
def order_authors_by_date(authors: dict) -> list:
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

######################################################
def patch_header(file: str) -> None:
    # extract log
    blame_log = git_load_blame_log(file)

    # extract authors
    authors = git_extract_authors_from_history(blame_log)
    last_edit = get_last_edit_month_year(blame_log)
    
    # as list
    authors_list = order_authors_by_date(authors)
    
    # build infos
    infos = build_infos(file)
    
    # get lang
    lang = calc_lang(file)

    # build header as list of lines
    header_script = build_new_file_header(authors_list, last_edit, infos, lang)

    # load file
    with open(file, 'r') as fp:
        original_content = fp.readlines()

    # patch
    patched_content = patch_header_lines(original_content, header_script, infos['width'], lang)

    # save again
    with open(file, 'w+') as fp:
        content = fp.writelines(patched_content)

######################################################
def patch_file(filename: str):
    # if exclude
    exclude = config['exclude_files']
    for pattern in exclude:
        if fnmatch.fnmatch(filename, pattern) or filename.startswith(pattern):
            print(f"{filename} : IGNORED")
            return
    
    # apply
    patch_header(filename)

######################################################
def main():
    # define arguments
    parser = argparse.ArgumentParser()
    parser.add_argument('FILE', help='File to patch')

    # parse
    options = parser.parse_args()

    # apply
    patch_file(options.FILE)

######################################################
if __name__ == "__main__":
    main()
