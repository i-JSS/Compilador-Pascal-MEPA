#!/usr/bin/bash

if ! make release; then
    echo "$(tput setaf 1)Build failed$(tput sgr0)"
    exit 1
fi


echo "Running tests"
echo "-------------------------------------"
for test_file in ./tests/sintatico/*.in; do
    base_name=$(basename "$test_file" .in)
    expected_output_file="./tests/sintatico/$base_name.out"
    echo "Running test: $base_name"

    time_taken=$( { time ./out/sintatico "$test_file" >./temp 2>./temp-error ; } 2>&1 )

    error=$(<./temp-error)

    if diff -q -B -w --suppress-common-lines ./temp "$expected_output_file" > /dev/null; then
        echo "$(tput setaf 2)Test $test_file passed$(tput sgr0)"
    else
        echo "$(tput setaf 1)Test $test_file failed$(tput sgr0)"
        echo "$(tput setaf 6)Diff output:$(tput sgr0)"
        diff -B -w --suppress-common-lines ./temp "$expected_output_file"
        if [ -n "$error" ]; then
            echo "$(tput setaf 1)With error: $error$(tput sgr0)"
        fi
    fi


    echo "Time taken: $time_taken"
    echo "-------------------------------------"
done

rm ./temp ./temp-error
