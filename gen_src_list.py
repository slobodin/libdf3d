import os, fnmatch

path = '.'

src_list, header_list = [], []

for root, dirs, files in os.walk(path):
    for name in files:
        if fnmatch.fnmatch(name, '*.cpp'):
            src_list.append(os.path.join(root, name))
        if fnmatch.fnmatch(name, '*.h'):
            header_list.append(os.path.join(root, name))

print('# NOTE: generated\n')

def print_list(lst_name, lst):
    print('set(' + lst_name)

    for name in lst:
        name = '    ${PROJECT_SOURCE_DIR}' + name[1:]
        name = name.replace('\\', '/')
        print(name)

    print(')\n')

print_list('libdf3d_HEADER_LIST', header_list)
print_list('libdf3d_SRC_LIST', src_list)
