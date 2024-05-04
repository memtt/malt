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
import os
import datetime
import argparse
# git
from git import Repo

######################################################
config = {
    "header_keys": {
        "PROJECT": "MALT (MALoc Tracker)",
        "VERSION": "1.2.2",
        "DATE": "@LAST_EDIT_MONTH_YEAR@",
        "LICENSE": "CeCILL-C"
    },
    "width": 55,
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

    # loop on all entries to extract the infos
    for entry in blame:
        parts = entry.split(' ', maxsplit=1)
        key = parts[0]
        if not ' ' in entry:
            commit_hash = entry
        if key == 'author-mail':
            commiter_mail = parts[1]
        if key == 'author':
            commiter_name = parts[1]
        elif key == 'author-time':
            commiter_time = parts[1]
        elif key == 'author-tz':
            commiter_zone = parts[1]
            log.append({'mail': commiter_mail, 'name': commiter_name, 'time': commiter_time, 'zone': commiter_zone, 'hash': commit_hash})

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
        # skip some
        if entry['hash'] in exclude_hashes:
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
def build_new_file_header_shell(authors_list: list, last_edit: str, infos: dict) -> list:   
    # build new header
    header_script = []
    width = infos['width']
    header_script.append( f'{"#"*width}\n')
    for key, value in infos['header'].items():
        fixed_value = value.replace('@LAST_EDIT_MONTH_YEAR@', last_edit)
        header_script.append(f'#    {key: <8} : {fixed_value}\n')
    header_script.append( f'#{"-"*(width-1)}\n')

    # add authors
    for author in authors_list:
        full_name = author['full-name']
        start = author['year-start']
        end = author['year-end']
        if start == end:
            years = str(start)
        else:
            years = f"{start} - {end}"
        header_script.append(f'#    AUTHOR   : {full_name} - {years}\n')
        
    # origin
    if infos['origin']:
        header_script.append( '#' + " ORIGIN ".center(width-1, '-')+'\n')
        for key, value in infos['origin'].items():
            header_script.append(f'#    {key: <8} : {value}\n')

    # close
    header_script.append( f'{"#"*width}\n')

    # ok
    return header_script

######################################################
def build_new_file_header(authors_list: list, last_edit:str, infos: dict, filename: str) -> list:

    if filename.endswith(".sh"):
        return build_new_file_header_shell(authors_list, last_edit, infos)
    elif os.path.basename(filename) in ["CMakeLists.txt", "options", "configure"]:
        return build_new_file_header_shell(authors_list, last_edit, infos)
    if filename.endswith(".py"):
        return build_new_file_header_shell(authors_list, last_edit, infos)
    else:
        raise Exception(f"Invalid file type : {filename}")

######################################################
def patch_header_lines(origin_content: list, new_header: list, width: int) -> list:
    # build patched
    patched_content = None

    # loop on content to find the start of header
    for line, value in enumerate(origin_content):
        if value == f'{"#"*width}\n' and ('#    PROJECT  : ' in origin_content[line + 1] or '#            PROJECT  : MA' in origin_content[line + 1]):
            count = 1
            while origin_content[line + count] != f'{"#"*width}\n':
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

    # build header as list of lines
    header_script = build_new_file_header(authors_list, last_edit, infos, file)

    # load file
    with open(file, 'r') as fp:
        original_content = fp.readlines()

    # patch
    patched_content = patch_header_lines(original_content, header_script, infos['width'])

    # save again
    with open(file, 'w+') as fp:
        content = fp.writelines(patched_content)

######################################################
def patch_file(filename: str):
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
