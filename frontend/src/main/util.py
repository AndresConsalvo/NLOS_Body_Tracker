# https://stackoverflow.com/questions/60656088/how-to-get-wireless-lan-adapter-wi-fi-ip-address-in-python, kind of hackish.
def wlan_ip():
    import subprocess
    result=subprocess.run('ipconfig',stdout=subprocess.PIPE,text=True).stdout.lower()
    scan=0
    for i in result.split('\n'):
        if 'wi-fi' in i: scan=1
        if scan:
            if 'ipv4' in i: return i.split(':')[1].strip()