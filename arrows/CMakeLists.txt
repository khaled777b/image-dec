cmake_minimum_required( VERSION 2.8 )

project( Arrows CXX )

include( FindBoost )

find_package( Boost 1.48.0 REQUIRED )

include_directories( ${Boost_INCLUDE_DIRS} )

add_executable( test_arrows arrow_test.cpp )