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

- ./fetchmail -f Test -p pass -u test@comp30023 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-ed512.out
- ./fetchmail -f Test -p pass -u test@comp30023 -n 2 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-mst.out
- ./fetchmail -f Test -u test@comp30023 -p pass1 -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-loginfail.out
- ./fetchmail -u test@comp30023 -p pass -n 1 -f Test1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-nofolder.out
- ./fetchmail -n 42 -u test@comp30023 -p pass -f Test retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-nomessage.out
- ./fetchmail -u test.test@comp30023 -p -p -f Test -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-mst.out
- ./fetchmail -f 'With Space' -n 1 -u test@comp30023 -p pass retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-mst.out

### Parse

- ./fetchmail -f Test -p pass -n 2 -u test@comp30023 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-mst.out
- ./fetchmail -f Test -n 3 -p pass -u test@comp30023 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-minimal.out
- ./fetchmail -p pass -f headers -u test@comp30023 -n 2 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-caps.out
- ./fetchmail -f headers -u test@comp30023 -p pass -n 3 parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-nosubj.out
- ./fetchmail -u test@comp30023 -n 4 -p pass -f headers parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-nested.out
- ./fetchmail -f headers -u test@comp30023 -n 5 -p pass parse unimelb-comp30023-2024.cloud.edu.au | diff - out/parse-ws.out

### MIME

- ./fetchmail -n 1 -p pass -u test@comp30023 mime unimelb-comp30023-2024.cloud.edu.au | diff - out/mime-ed512.out
- ./fetchmail -f Test -n 2 -p pass -u test@comp30023 mime unimelb-comp30023-2024.cloud.edu.au | diff - out/mime-mst.out

### List

- ./fetchmail -p pass -u test@comp30023 -f Test list unimelb-comp30023-2024.cloud.edu.au | diff - out/list-Test.out
- ./fetchmail -p pass -u test@comp30023 list unimelb-comp30023-2024.cloud.edu.au | diff - out/list-INBOX.out

### TLS (Stretch Goal)

- ./fetchmail -f Test -p pass -u test@comp30023 -n 1 -t retrieve unimelb-comp30023-2024.cloud.edu.au | diff - out/ret-ed512.out

## Debugging

valgrind -s --leak-check=full --show-leak-kinds=all ./fetchmail -u test@comp30023 -p pass -f Test -n 1 retrieve unimelb-comp30023-2024.cloud.edu.au
