import requests

def formatURLforCommand(url):
    return "http://"+url+":80/command"

def turn_left(url):
    print(f"Turning {formatURLforCommand(url)} left, captain!")
    resp = requests.post(url=formatURLforCommand(url))
    return 0

