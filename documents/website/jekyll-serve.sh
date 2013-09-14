#!/bin/sh
#
# sudo apt-get install rubygems
# sudo gem install jekyll
# sudo gem install rdiscount
#

# Start jekyll server => Now browse to http://localhost:4000
WEBSITE=$(cd `dirname $0` ; pwd)
jekyll serve --watch --source ${WEBSITE} --destination ${WEBSITE}/_site
