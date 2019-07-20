#!/bin/bash
if [[ "$OSTYPE" == "linux-gnu" ]]; then
    ./tools/bin/linux/genie vs2017
elif [[ "$OSTYPE" == "darwin"* ]]; then
    # Mac OSX
    ./tools/bin/macosx/genie vs2017
fi

