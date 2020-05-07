# Copyright (C) 2014-2020  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

###
## The script collects information about help documents in plugins
## and prepares building of help documentation by sphinx
###

import argparse
import os
import os.path as osp
import re
import shutil
import sys
from xml.dom.minidom import parse

def find_dir(src_path, cfg_file):
    """Find a name of a directory where the given config file exists"""
    entities = os.listdir(src_path)
    for entity in entities:
        path = osp.join(src_path, entity)
        cfg_path = osp.join(path, cfg_file)
        if not osp.isdir(path):
            continue
        if not osp.exists(cfg_path) and not osp.exists(cfg_path + '.in'):
            continue
        if osp.exists(osp.join(path, 'doc')):
            return osp.join(path, 'doc')
    return None

def main(src_dir, build_dir):
    """Main function"""
    src_path = osp.realpath(osp.join(src_dir, *[os.pardir]*2, 'src'))
    config_file = osp.join(src_path, 'Config', 'plugins.xml.in')

    ## Find accessible plugins from plugins.xml configuration file
    processed = []

    indices = []
    tui_scripts = []

    doc = parse(config_file)
    plugins = doc.getElementsByTagName('plugin')
    for plugin in plugins:
        plugin_name = plugin.getAttribute('library')
        if not plugin_name:
            plugin_name = plugin.getAttribute('script')
        if not plugin_name or plugin_name in processed:
            continue
        config = plugin.getAttribute('configuration')
        if not config:
            continue
        lib_dir = find_dir(src_path, config)
        if lib_dir:
            ## Copy plugin's documenation folder to the build directory
            dist_dir = osp.join(build_dir, plugin_name)
            if osp.exists(dist_dir):
                shutil.rmtree(dist_dir)
            shutil.copytree(lib_dir, dist_dir)
            ## Collect index file
            indices.append(osp.join(plugin_name, plugin_name + '.rst'))
            ## Collect TUI scripts
            tui_files = sorted(f for f in os.listdir(lib_dir) if \
                                   osp.isfile(osp.join(lib_dir, f)) and re.match('TUI_.*\.rst', f))
            if tui_files:
                tui_list_file = osp.join(dist_dir, 'TUI_examples.rst')
                with open(tui_list_file, 'w') as flist:
                    flist.write('.. _tui_{}:\n\n'.format(plugin_name.lower()))
                    title = plugin_name.replace('Plugin', ' plug-in')
                    flist.write('{}\n'.format(title))
                    flist.write('{}\n\n'.format('='*len(title)))
                    flist.write('.. toctree::\n')
                    flist.write('   :titlesonly:\n')
                    flist.write('   :maxdepth: 1\n\n')
                    flist.writelines(['   {}\n'.format(i) for i in tui_files])
                tui_scripts.append(osp.join('..', plugin_name, 'TUI_examples.rst'))
            ## Mark plugin as processed
            processed.append(plugin_name)

    ## Generate index file
    in_file = osp.join(src_dir, 'index.rst.in')
    out_file = osp.join(build_dir, 'index.rst')
    with open(in_file, 'r') as fin, open(out_file, 'w') as fout:
        lines = fin.readlines()
        idx = lines.index('<insert here>\n')
        lines = lines[:idx] + ['   {}\n'.format(i) for i in indices] + lines[idx+1:]
        fout.writelines(lines)

    ## Generate TUI scripts index
    in_file = osp.join(src_dir, 'TUI_scripts.rst.in')
    out_file = osp.join(build_dir, 'General', 'TUI_scripts.rst')
    with open(in_file, 'r') as fin, open(out_file, 'w') as fout:
        lines = fin.readlines()
        idx = lines.index('<insert here>\n')
        lines = lines[:idx] + ['   {}\n'.format(i) for i in tui_scripts] + lines[idx+1:]
        fout.writelines(lines)

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Generate index file from source directory')
    parser.add_argument('build_dir', help='build directory')
    parser.add_argument('src_dir', help='source directory')
    args = parser.parse_args()
    sys.exit(main(args.src_dir, args.build_dir))
