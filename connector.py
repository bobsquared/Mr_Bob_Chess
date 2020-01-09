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
    start = time.time() * 1000
    bestMove = ""

    s = "go " + str(depth)
    print(s)
    engine.stdin.write(s + "\n")  # Include '\n'
    engine.stdin.flush()
    # engine.stdin.flush()
    while True:

        end = time.time() * 1000

        if (end - start) > 10000000:
            return bestMove



        response = engine.stdout.readline().strip()

        if response.startswith("Best move found:"):
            bestMove = response[17:21]
            print(response)
            # print(bestMove)
        elif response == 'end':
            break
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
