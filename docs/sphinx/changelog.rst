.. _changelog:


#########
Changelog
#########

******************
1.2.1 (unreleased)
******************

Internal Changes
================
- Replace shell scripts with inline Gitlab CI configuration (:issue:`2`, :merge:`12`). By `Kyle Brindley`_.
- Create project specific CI environment (:issue:`3`, :merge:`13`). By `Kyle Brindley`_.
- Use setuptools_scm for Git tag versioning (:issue:`4`, :merge:`14`). By `Kyle Brindley`_.
- Conda package and deployment (:issue:`5`, :merge:`15`). By `Kyle Brindley`_.

******************
1.1.1 (2022-11-03)
******************

Internal Changes
================

- Fixed bug in linear elastic constraint equations (:merge:`7`). By `Nathan Miller`_.
- Fixed additional bug in linear elastic constraint equations (:merge:`8`). By `Nathan Miller`_.

******************
1.1.0 (08-16-2022)
******************

Internal Changes
================

- Moved the code to the cpp_stub format (:merge:`1`). By `Nathan Miller`_.
- Moved the tests to the BOOST test format (:merge:`2`). By `Nathan Miller`_.
- Removed old material library interface definitions (:merge:`3`). By `Nathan Miller`_.
- Added the ability to turn of building the python bindings (:merge:`4`). By `Nathan Miller`_.
- Added wrapper for calculation of current stresses from the fundamental deformation measures (:merge:`5`). By `Nathan Miller`_.

Release
=======

- Released version 1.1.0 (:merge:`6`). By `Nathan Miller`_.
