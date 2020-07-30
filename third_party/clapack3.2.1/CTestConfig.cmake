## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   include(CTest)
set(CTEST_PROJECT_NAME "CLAPACK")
set(CTEST_NIGHTLY_START_TIME "00:00:00 EDT")

set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "my.cdash.org")
set(CTEST_DROP_LOCATION "/submit.php?project=CLAPACK")
set(CTEST_DROP_SITE_CDASH TRUE)
