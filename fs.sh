#!/bin/bash

test1() {
    if ! ./bin/dir_test; then
        handle_error
    else
        echo '\n\033[1;32mTest 1 completed: Directory and file creation successfully executed.\033[0m\n'
    fi
}

test2() {
    if ! ./bin/write_read_test; then
        handle_error
    else
        echo '\n\033[1;32mTest 2 completed: File write and read successfully executed.\033[0m\n'
    fi
}

test3() {
    if ! ./bin/seek_test; then
        handle_error
    else
        echo '\n\033[1;32mTest 3 completed: File write and read with seek successfully executed.\033[0m\n'
    fi
}

test4() {
    if ! ./bin/erase_test; then
        handle_error
    else
        echo '\n\033[1;32mTest 4 completed: Directory and file deletion successfully executed.\033[0m\n'
    fi
}

test5() {
    if ! ./bin/dir_test || ! ./bin/write_read_test || ! ./bin/seek_test || ! ./bin/erase_test; then
        handle_error
    else
        echo '\n\033[1;32mAll tests successfully completed.\033[0m\n'
    fi
}

handle_error() {
    echo '\n\033[1;31mError: File not found.\033[0m'
    echo '\nAre you sure you have compiled the files?'
    echo "1. Yes, I have compiled"
    echo "2. No, I have not compiled"
    read -p "Enter your choice: " compile_choice

    case $compile_choice in
        1)
            echo '  ______________________________________________'
            echo ' /                                              \\'
            echo ' |\033[1;31m                  Don'"'"'t Lie !!!           \033[0m    |'
            echo ' \\________              ________________________/'
            echo '          \\_         __/    ___---------__       '
            echo '            \\      _/      /              \\     '
            echo '             \\    /       /                 \\    '
            echo '              |  /       | _    _ \\          \\   '
            echo '              | |       / / \\  / \\ |          \\  '
            echo '              | |       ||   ||   ||           | '
            echo '              | |       | \\_//\\\\_/ |           |'
            echo '              | |       |_| (||)   |_______|   |'
            echo '              | |         |  ||     | _  / /   |'
            echo '               \\ \\        |_________|| \\/ /   /'
            echo '                \\ \\_       |_|_|_|_|/|  _/___/ '
            echo '                 \\__>       _ _/_ _ /  |       '
            echo '                          .|_|_|_|_|   |       '
            echo '                          |           /        '
            echo '                          |__________/         '
            echo ''
            exit 1
            ;;
        2)
            echo '\nRunning make...\n'
            make
            ;;
        *)
            echo '\n\033[1;31mInvalid choice. Exiting.\033[0m\n'
            exit 1
            ;;
    esac
}

test() {
    while true; do
        echo '\nWhich test do you want to execute?'
        echo "1. Directory and file creation (recommended first)"
        echo "2. File write and read (recommended only if test 1 has been executed)"
        echo "3. File write and read with seek (recommended only if test 1 has been executed)"
        echo "4. Directory and file deletion (recommended only if test 1 has been executed)"
        echo "5. All tests"
        echo "q. Quit"
        read -p "Enter your choice: " test_choice

        case $test_choice in
            1) test1 ;;
            2) test2 ;;
            3) test3 ;;
            4) test4 ;;
            5) test5 ;;
            q)
                echo '\n\033[1;32mQuitting ...\033[0m\n'
                break
                ;;
            *) echo '\n\033[1;31mInvalid choice. Please try again.\033[0m\n' ;;
        esac
    done
}

handle_error_shell() {
    echo '\n\033[1;31mError: File not found.\033[0m'
    echo '\nAre you sure you have compiled the files?'
    echo "1. Yes, I have compiled"
    echo "2. No, I have not compiled"
    read -p "Enter your choice: " compile_choice

    case $compile_choice in
        1)
            echo '  ______________________________________________'
            echo ' /                                              \\'
            echo ' |\033[1;31m                  Don'"'"'t Lie !!!           \033[0m    |'
            echo ' \\________              ________________________/'
            echo '          \\_         __/    ___---------__       '
            echo '            \\      _/      /              \\     '
            echo '             \\    /       /                 \\    '
            echo '              |  /       | _    _ \\          \\   '
            echo '              | |       / / \\  / \\ |          \\  '
            echo '              | |       ||   ||   ||           | '
            echo '              | |       | \\_//\\\\_/ |           |'
            echo '              | |       |_| (||)   |_______|   |'
            echo '              | |         |  ||     | _  / /   |'
            echo '               \\ \\        |_________|| \\/ /   /'
            echo '                \\ \\_       |_|_|_|_|/|  _/___/ '
            echo '                 \\__>       _ _/_ _ /  |       '
            echo '                          .|_|_|_|_|   |       '
            echo '                          |           /        '
            echo '                          |__________/         '
            echo ''
            exit 1
            ;;
        2)
            echo '\nRunning make...\n'
            make
            shell
            ;;
        *)
            echo '\n\033[1;31mInvalid choice. Exiting.\033[0m\n'
            exit 1
            ;;
    esac
}

shell() {
    clear
    
    if ! ./shell/main_shell; then
        handle_error_shell
    fi
}
while true; do
    echo '\nWhat do you want to do?'
    echo "1. View the tests"
    echo "2. Start the shell"
    echo "c. Compile the files (recommended first)"
    echo "q. Quit"
    read -p "Enter your choice: " main_choice

    case $main_choice in
        1) test ;;
        2) shell ;;
        c) make ;;
        q) 
            echo '\n\033[1;32mQuitting ...\033[0m\n'
            break;;
        *) echo $'\n\033[1;31mInvalid choice. Exiting.\033[0m\n' ;;
    esac
done
