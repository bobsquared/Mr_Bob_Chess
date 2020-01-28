import subprocess
import time
import config

def put(command):

    engine.stdin.write(command + "\n")
    engine.stdin.flush()

    while True:

        s = "isready"
        engine.stdin.write(s + "\n")
        engine.stdin.flush()
        response = engine.stdout.readline().strip()
        if response.startswith("readyok"):
            # print ("Process response:", response)
            return
        else:

            break

def search(depth):
	# using the 'isready' command (engine has to answer 'readyok')
	# to indicate current last line of stdout

    bestMove = ""

    s = "go wtime 1500 btime 1500"
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
        elif response != '':
            i = 0
            print ("Process response:", response)
        else:
            break


    return bestMove



engine = subprocess.Popen(
	'Mr Bob.exe',
	universal_newlines = True,
	stdin=subprocess.PIPE,
	stdout=subprocess.PIPE,
)
