import os.path
import csv
import random

def makeIncompleteDatabase(path, rate):
    complete_file = open(path)
    basename = os.path.basename(path)
    new_name = basename.split('.')[0] + '_' + str(rate) + '.csv'
    new_path = os.path.dirname(path) + '/' + new_name
    incomplete_file = open(new_path, 'w')
    csv_r = csv.reader(complete_file)
    csv_w = csv.writer(incomplete_file)
    headers = next(csv_r)
    csv_w.writerow(headers)
    for row in csv_r:
        if random.random() <= rate:
            col = random.randint(0, len(row) - 1)
            row[col] = None
            csv_w.writerow(row)
        else:
            csv_w.writerow(row)
    complete_file.close()
    incomplete_file.close()

makeIncompleteDatabase('../archive/teams.csv', 0.5)




