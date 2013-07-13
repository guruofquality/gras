# Copyright (C) by Josh Blum. See LICENSE.txt for licensing information.

import os
import sys
from Cheetah.Template import Template

MAX_ARGS = 11

def expand(t, n):
    out = list()
    for i in range(n): out.append(t.replace('%d', str(i)))
    return ', '.join(out)

def cleanup(code):
    code = code.replace('template <>', 'inline')
    code = code.replace(', >', '>')
    code = code.replace(', )', ')')
    code = code.replace('\\#', '#')
    return code

if __name__ == '__main__':
    in_path = sys.argv[1]
    out_path = sys.argv[2]
    tmpl = open(in_path, 'r').read()
    for key in ['define', 'include', 'if', 'endif', 'else', 'ifdef', 'ifndef', 'pragma']:
        tmpl = tmpl.replace('#%s'%key, '\\#%s'%key)
    code = str(Template(tmpl, dict(
        MAX_ARGS=MAX_ARGS, expand=expand,
    )))
    code = cleanup(code)
    if not os.path.exists(out_path) or open(out_path, 'r').read() != code:
        print 'write code to', out_path
        open(out_path, 'w').write(code)
