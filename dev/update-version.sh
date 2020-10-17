#!/bin/bash
######################################################
#            PROJECT  : MATT                         #
#            VERSION  : 1.2.2                        #
#            DATE     : 10/2020                      #
#            AUTHOR   : Valat Sébastien              #
#            LICENSE  : CeCILL-C                     #
######################################################

######################################################
COLOR_RED="$(printf "\033[31m")"
COLOR_RESET="$(printf "\033[0m")"

######################################################
function extract_old_version()
{
	OLD_VERSION=$(cat configure | grep VERSION | xargs echo | cut -d " " -f 4)
	OLD_VERSION_PROTECTED=$(echo $OLD_VERSION | sed -e 's/\./\\./g')
	OLD_SHORT_VERSION=$(echo $OLD_VERSION | sed -e 's/-dev//g')
	OLD_SHORT_VERSION_PROTECTED=$(echo $OLD_SHORT_VERSION | sed -e 's/\./\\./g')
	echo "Old version is ${OLD_VERSION}, short is ${OLD_SHORT_VERSION}"
}

######################################################
function print_error()
{
	echo "${COLOR_RED}$@${COLOR_RESET}" 1>&2
}

######################################################
#check args
if [ -z "$1" ]; then
	print_error "Missing version !"
	echo "Usage : $0 {VERSION}" 1>&2
	exit 1
fi

######################################################
#check call from root directory of project
if [ "$0" != "./dev/update-version.sh" ]; then
	print_error "Caution, you must call this script from the project root directory !"
fi

######################################################
version="$1"
shortVersion="$(echo $1 | sed -e 's/-dev//g')"
newdate="DATE     : $(date +%m/%Y)"
V="VERSION"
newversion="$V  : $(printf "%-16s" "$version")"

######################################################
extract_old_version
echo "New version is ${newversion}"
echo "Date is ${newdate}"

######################################################
echo
echo
for tmp in $(seq 1 5); do
	echo "Will start in $((5 - $tmp)) seconds, CTRL+C to stop"
	sleep 1
done


######################################################
update_all_files()
{
	find ./ | while read file
	do
		#exclude directories
		if [ -d "$file" ]; then continue; fi
		if [ -L "$file" ]; then continue; fi

		#exclude some paths
		case "$file" in
			*node_modules*|*bower_components*|*.git*|*extern-deps*)
				#echo "exclude $file"
				continue
				;;
		esac

		#exclude non MALT files
		if [ -z "$(cat $file | grep MALT)" ] && [ -z "$(cat $file | grep malt)" ]
		then
			continue;
		fi

		#do replacement
		sed -i -r -e "s#  DATE     : [0-9]{2}/[0-9]{4}#  ${newdate}#g" \
	        	  -e "s#  $V  : .{16}#  ${newversion}#" \
		          -e "s#  $V  : [0-9a-zA-Z.-]+\$#  $V  : ${version}#" \
				  "${file}"
	done
}

######################################################
#update bower and package
update_special_sed()
{
	sed -i -r -e "s/$OLD_VERSION_PROTECTED/${version}/g" src/integration/malt.sh.in
	sed -i -r -e "s/\"version\": \"[0-9A-Za-z.-]+\"/\"version\": \"${shortVersion}\"/g" src/webview/bower.json
	sed -i -r -e "s/\"version\": \"[0-9A-Za-z.-]+\"/\"version\": \"${shortVersion}\"/g" src/webview/package.json
	sed -i -r -e "s/^version=[0-9A-Za-z.-]+$/version=${version}/g" dev/gen-archive.sh
	sed -i -r -e "s/${OLD_SHORT_VERSION_PROTECTED}/${shortVersion}/g" packaging/README.md
	sed -i -r -e "s/${OLD_SHORT_VERSION_PROTECTED}/${shortVersion}/g" dev/packaging.sh
	sed -i -r -e "s/${OLD_SHORT_VERSION_PROTECTED}/${shortVersion}/g" packaging/fedora/malt.spec
	sed -i -r -e "s/${OLD_SHORT_VERSION_PROTECTED}/${shortVersion}/g" packaging/debian/changelog
	sed -i -r -e "s/${OLD_SHORT_VERSION_PROTECTED}/${shortVersion}/g" src/manpages/*.ronn
	sed -i -r -e "s/${OLD_SHORT_VERSION_PROTECTED}/${shortVersion}/g" packaging/gentoo/dev-util/malt/malt-*.ebuild
	sed -i -r -e "s/${OLD_SHORT_VERSION_PROTECTED}/${shortVersion}/g" packaging/archlinux/PKGBUILD
	sed -i -r -e "s/${OLD_SHORT_VERSION_PROTECTED}/${shortVersion}/g" Doxyfile
	sed -i -r -e "s/${OLD_SHORT_VERSION_PROTECTED}/${shortVersion}/g" src/lib/CMakeLists.txt
	sed -i -r -e "s/${OLD_VERSION_PROTECTED}/${version}/g" src/lib/tests/TestValgrindOutput.cpp
	sed -i -r -e "s/${OLD_VERSION_PROTECTED}/${version}/g" .vscode/snippets/cpp.json
	sed -i -r -e "s/${OLD_VERSION_PROTECTED}/${version}/g" src/lib/tests/TestValgrindOutput.cpp
	sed -i -r -e "s/${OLD_VERSION_PROTECTED}/${version}/g" CMakeLists.txt
}

######################################################
update_packaging()
{
	#update date in debian changelog
	sed -i -r -e "s/ [A-Z][a-z]{2}, [0-9]+ [A-Z][a-z]{2} [0-9]+ [0-9]{2}:[0-9]{2}:[0-9]{2} \+[0-9]{4}$/$(LANG=C date -R)/g" packaging/debian/changelog

	#mv gentoo ebuild
	git mv packaging/gentoo/dev-util/malt/malt-${OLD_VERSION}.ebuild packaging/gentoo/dev-util/malt/malt-${version}.ebuild || exit 1
}

######################################################
#Check if manpages need to be recompiled
update_manpages()
{
	make -C ./src/manpages || exit 1
}

######################################################
#serach not updated
post_check()
{
	echo "=================== Check not updated list ======================"
	grep -RHn "$(echo "${OLD_VERSION}" | sed -e 's/\./\\./g')" ./ | grep -v node_modules | grep -v extern-deps | grep -v "\.git" | grep -v bower_components | grep -v ChangeLog.md | grep -v build | grep "${OLD_VERSION}"
	if [ "$shortVerison" != "$OLD_SHORT_VERSION" ]; then
		grep -RHn "${OLD_SHORT_VERSION_PROTECTED}" ./ | grep -v node_modules | grep -v extern-deps | grep -v "\.git" | grep -v bower_components | grep -v ChangeLog.md | grep -v build | grep "${OLD_SHORT_VERSION}"
	fi
	echo "================================================================="
}

update_all_files
update_special_sed
update_manpages
post_check

echo "TODO: Think to update changelog date or automate"
