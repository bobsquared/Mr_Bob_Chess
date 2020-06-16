import subprocess
import time
import config
import re

cp = re.compile('.*cp\s+(-?\d+)')

def put(command):

    engine.stdin.write(command + "\n")
    engine.stdin.flush()
    count = 0

    if command.startswith("color") or command.startswith("position startpos moves"):
        return

    while True:
        response = engine.stdout.readline().strip()
        if response.startswith("-"):
            print (response)
            count += 1
            if count == 6:
                return
        else:
            print(response)

def search(depth):
	# using the 'isready' command (engine has to answer 'readyok')
	# to indicate current last line of stdout

    bestMove = ""
    centipawn = ""

    s = "go wtime 25000 btime 25000"
    print(s)
    engine.stdin.write(s + "\n")  # Include '\n'
    engine.stdin.flush()
    # engine.stdin.flush()
    while True:

        response = engine.stdout.readline().strip()

        if response.startswith("bestmove "):
            bestMove = response[9:13]
            print(response)
            break
            # print(bestMove)
        elif response.startswith("info "):
            print(response)
            m = re.match(cp, response)
            centipawn = m.group(1)
        elif response != '':
            i = 0
            print ("Process response:", response)
        else:
            break


    return bestMove, centipawn



engine = subprocess.Popen(
	'Mr Bob.exe',
	universal_newlines = True,
	stdin=subprocess.PIPE,
	stdout=subprocess.PIPE,
)
