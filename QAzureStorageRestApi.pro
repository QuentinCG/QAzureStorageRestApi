TEMPLATE = subdirs

CONFIG += ordered

SUBDIRS = lib \
          example \
          test \

lib.subdir = lib
example.subdir = example
test.subdir = test

example.depends = lib
test.depends = lib
