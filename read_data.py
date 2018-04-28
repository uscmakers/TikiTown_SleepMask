import csv

def read_data(filename):
	with open('data.csv', 'rb') as csvfile:
		spamreader = csv.reader(csvfile)
		tally_highs = 0;
		for row in spamreader:
			if (float(row[2]) >= 1):
				tally_highs = tally_highs + 1
			else:
				tally_highs = 0;
			if (tally_highs >= 5):
				print row[0]

if __name__ == '__main__':
	read_data('data.csv')
