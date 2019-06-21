# Copyright (c) 2017 Sony Corporation. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

from collections import OrderedDict
from os.path import basename, abspath, dirname, join, splitext
import importlib
import json
import os
import re
import subprocess
import yaml
import zlib


def represent_odict(dumper, instance):
    return dumper.represent_mapping('tag:yaml.org,2002:map', instance.items())


yaml.add_representer(OrderedDict, represent_odict)


def load_yaml_ordered(stream, Loader=yaml.Loader, object_pairs_hook=OrderedDict):
    '''
    Load function with keeping the order of dictionaries.
    '''
    class OrderedLoader(Loader):
        pass

    def construct_mapping(loader, node):
        loader.flatten_mapping(node)
        return object_pairs_hook(loader.construct_pairs(node))
    OrderedLoader.add_constructor(
        yaml.resolver.BaseResolver.DEFAULT_MAPPING_TAG,
        construct_mapping)
    return yaml.load(stream, OrderedLoader)


class CodeGenerator:
    def __init__(self):
        self.get_function_info()

    def get_function_info(self):
        with open(join(dirname(abspath(__file__)),
                       'functions.yaml'), 'r') as f:
            functions_yaml = f.read()
            self.info = load_yaml_ordered(functions_yaml)
            self.version = zlib.crc32(
                functions_yaml.encode('utf-8')) & 0x7ffffff

    def generate(self, name):
        output_filename_tmp = abspath(join(
            dirname(abspath(__file__)), '..', '..', name + '_tmp'))
        output_filename = abspath(join(
            dirname(abspath(__file__)), '..', '..', name))

        generator_module = 'generators.generator_' + \
            name.replace('/', '_').replace('.', '_')
        generator = importlib.import_module(generator_module)
        contents = generator.generate(abspath(join(
            dirname(abspath(__file__)), 'generators', basename(name) + '.tmpl')), self.info)

        with open(output_filename_tmp, 'w', encoding='utf-8') as f:
            f.write(contents)

        if splitext(output_filename)[1] in ['.c', '.h']:
            subprocess.run(
                ['clang-format', '-i', '--style=llvm', output_filename_tmp])

        if os.path.exists(output_filename):
            try:
                subprocess.run(
                    ['diff', '-q', output_filename_tmp, output_filename], check=True, stdout=subprocess.PIPE)
            except subprocess.CalledProcessError:
                print('Generated [{}].'.format(basename(output_filename)))
                for l in subprocess.run(
                        ['diff', output_filename_tmp, output_filename], check=False, stdout=subprocess.PIPE).stdout.decode('utf-8').split('\n'):
                    if l:
                        print(l)
                os.unlink(output_filename)
                os.rename(output_filename_tmp, output_filename)
            finally:
                if os.path.exists(output_filename_tmp):
                    os.unlink(output_filename_tmp)
        else:
            os.rename(output_filename_tmp, output_filename)
            print('Generated [{}].'.format(basename(output_filename)))

    def generate_all(self):
        self.generate('doc/SUPPORT_STATUS.md')
        self.generate('include/nnablart/network.h')
        self.generate('include/nnablart/functions.h')
        self.generate('src/nnablart/dump_function.c')
        self.generate('src/runtime/function_context.c')
        self.generate('include/nnablart/config.h')


CodeGenerator().generate_all()
