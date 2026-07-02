from alive_progress import alive_bar

import time

with alive_bar(100, title='Writing to EEPROM', bar='blocks', stats=False, monitor=False) as bar:
    for i in range(100):
        time.sleep(0.05)
        bar.title(f'Processing item {i + 1}')
        bar()
