# COMP30023 2024 - Project 2

## Format

./fetchmail -u \<username\> -p \<password\> [-f \<folder\>] [-n \<messageNum\>] [-t] \<command\> \<server_name\>

([ ] means args are optional)

### Commands

- retrieve
- parse
- mime
- list

## Testing

### Retrieve

./fetchmail -f Test -p pass -u test@comp30023 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-ed512.out
./fetchmail -f Test -p pass -u test@comp30023 -n 2 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-mst.out
./fetchmail -f Test -u test@comp30023 -p pass1 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-loginfail.out
./fetchmail -u test@comp30023 -p pass -n 1 -f Test1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-nofolder.out
./fetchmail -n 42 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-nomessage.out
./fetchmail -u test.test@comp30023 -p -p -f Test -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-mst.out
./fetchmail -f 'With Space' -n 1 -u test@comp30023 -p pass retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-mst.out

### Parse

./fetchmail -f Test -p pass -n 2 -u test@comp30023 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-mst.out
./fetchmail -f Test -n 3 -p pass -u test@comp30023 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-minimal.out
./fetchmail -p pass -f headers -u test@comp30023 -n 2 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-caps.out
./fetchmail -f headers -u test@comp30023 -p pass -n 3 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-nosubj.out
./fetchmail -u test@comp30023 -n 4 -p pass -f headers parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-nested.out
./fetchmail -f headers -u test@comp30023 -n 5 -p pass parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-ws.out

### MIME

./fetchmail -n 1 -p pass -u test@comp30023 mime unimelb-comp30023-2024.cloud.edu.au | diff - out/mime-ed512.out
./fetchmail -f Test -n 2 -p pass -u test@comp30023 mime unimelb-comp30023-2024.cloud.edu.au | diff - out/mime-mst.out

### List

./fetchmail -p pass -u test@comp30023 -f Test list unimelb-comp30023-2024.cloud.edu.au | diff - out/list-Test.out
./fetchmail -p pass -u test@comp30023 list unimelb-comp30023-2024.cloud.edu.au | diff - out/list-INBOX.out

## Debugging

valgrind -s --leak-check=full --show-leak-kinds=all ./fetchmail -u test@comp30023 -p pass -f Test -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au

## Other tests

./fetchmail
./fetchmail -u
./fetchmail retrieve
./fetchmail unimelb-comp30023-2024.cloud.edu.au
./fetchmail retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -p -u retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -p a -u f -f retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n 0 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n -1 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n 10000000000000000000000000000000000 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n comp30023 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n 1 -u test@comp30023 -p pass -f '' retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n 1 -u test@comp30023 -p pass -f ' ' retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -n 1 -u test@comp30023 -p pass -f '🫤' retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -u f -p a retrieve ''
./fetchmail -u f -p a retrieve ' '
./fetchmail -u f -p a retrieve aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa
./fetchmail -f Test -p $'pass\r\nCOMP30023 LOGIN inject@comp30023 pass' -u inject@comp30023 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -f Test -p pass -u test@comp30023 -n 1 retrieve <a-server-which-disconnects>

./fetchmail -f Test -p pass -u test@comp30023 -n 1 -t retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-ed512.out

./fetchmail -f more -p pass -u test@comp30023 -n 2 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-nul.out
./fetchmail -f more -p pass -u test@comp30023 -n 1 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-mst-tab.out
./fetchmail -u test@comp30023 -p pass -f Test -n '1:2' retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -u test@comp30023 -p pass -f Test -n $'1\x2c2' retrieve unimelb-comp30023-2024.cloud.edu.au
./fetchmail -u test@comp30023 -p pass -f Test -n '*' retrieve unimelb-comp30023-2024.cloud.edu.au