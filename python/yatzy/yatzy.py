import random
import sys
import os


# Clear the screen
def clear_screen():
	if os.name == 'nt':         # Windows
		os.system('cls')
	else:                       # Everyone else
		os.system('clear')


# Count the number of unique pairs in DICE
def number_of_pairs(dice):
	pairs = 0
	for i in range(1, 7):
		pairs += dice.count(i) >= 2
	return pairs


# Check if all the dice are valid
def dice_are_valid(dice):
	for x in dice:
		if x < 1 or x > 5:
			return False
	return True


# Calculate the score for CATEGORY with the given DICE
def calculate_score(category, dice):
	match category:
		case 'Ones':
			return dice.count(1)
		case 'Twos':
			return dice.count(2) * 2
		case 'Threes':
			return dice.count(3) * 3
		case 'Fours':
			return dice.count(4) * 4
		case 'Fives':
			return dice.count(5) * 5
		case 'Sixes':
			return dice.count(6) * 6
		case 'One Pair':
			for i in range(6, 0, -1):
				if dice.count(i) >= 2:
					return i * 2
			return 0
		case 'Two Pairs':
			if number_of_pairs(dice) != 2:
				return 0

			# Sum all of the pairs.  There are only 5 dice, so it is not
			# possible that we have more than 2 pairs.
			n = 0
			for i in range(6, 0, -1):
				if dice.count(i) >= 2:
					n += i * 2
			return n
		case 'Three of a Kind':
			for i in range(6, 0, -1):
				if dice.count(i) >= 3:
					return i * 3
			return 0
		case 'Four of a Kind':
			for i in range(6, 0, -1):
				if dice.count(i) >= 4:
					return i * 4
			return 0
		case 'Small Straight':
			if set(dice) == {1, 2, 3, 4, 5}:
				return 15
			return 0
		case 'Large Straight':
			if set(dice) == {2, 3, 4, 5, 6}:
				return 20
			return 0
		case 'Full House':
			# If there are 2 pairs, but only 2 different dice, it means (because
			# there are only 5 dice total) that we have a full house
			if len(set(dice)) == 2 and number_of_pairs(dice) == 2:
				return sum(dice)
			return 0
		case 'Chance':
			return sum(dice)
		case 'Yatzy':
			if len(set(dice)) == 1:
				return 50
			return 0


# Calculate the total score factoring in the bonus
def calculate_total_score(scores):
	n = sum(x for x in scores.values() if x != -1)
	upper_section = (
		scores['Ones'] +
		scores['Twos'] +
		scores['Threes'] +
		scores['Fours'] +
		scores['Fives'] +
		scores['Sixes']
	)
	if upper_section >= 63:
		n += 50
	return n


# Check if the game is over (because the player has filled in the entire score
# sheet)
def game_is_over(scores):
	return -1 not in scores.values()


# Main game loop
def main():
	print('Enter the players name or ‹ENTER› if all names have been entered.')

	# Read the players’ names
	names = []
	scores = {}

	while True:
		name = input('Name => ').strip()
		if name == '':
			break
		names.append(name)
		scores[name] = {
			'Ones':            -1,
			'Twos':            -1,
			'Threes':          -1,
			'Fours':           -1,
			'Fives':           -1,
			'Sixes':           -1,
			'One Pair':        -1,
			'Two Pairs':       -1,
			'Three of a Kind': -1,
			'Four of a Kind':  -1,
			'Small Straight':  -1,
			'Large Straight':  -1,
			'Full House':      -1,
			'Chance':          -1,
			'Yatzy':           -1,
		}

	if len(names) == 0:
		print('No players found!  Exiting…')
		sys.exit(0)

	# Check if the game is over using the score sheet of the first player
	while not game_is_over(scores[names[0]]):
		for player in names:
			# Initialize variables
			score = calculate_total_score(scores[player])
			rolls_left = 2
			dice = [random.randint(1, 6) for _ in range(5)]

			# Clear the screen and print a header, then ask which dice to reroll
			# and reroll if appropriate
			while rolls_left > 0:
				clear_screen()

				print(f'Current score for {player}: {score}')
				print(f'Your current dice: {dice}\n')
				print(f'Which dice (1–5) would you like to reroll?  (Rolls left: {rolls_left})')
				print('Enter the indicies space-separated or press ‹ENTER› to keep all\n')

				while True:
					xs = []
					s = input('Dice => ').strip()
					if s == '':
						break

					xs = s.split(' ')
					try:
						xs = [int(x) for x in xs]
					except ValueError:
						print('Invalid dice given.  Please try again!')
					else:       # If there was no exception
						if dice_are_valid(xs):
							break
						print('Invalid dice given.  Please try again!')

				if xs == []:
					break

				# We do x - 1 because the list starts at 0, but the user will be
				# counting from 1.
				for x in xs:
					dice[x - 1] = random.randint(1, 6)

				rolls_left -= 1

			# Show the possible categories that the player can select, and
			# prompt them for a category to pick
			clear_screen()
			player_scores = scores[player]

			print(f'Current score for {player}: {score}')
			print(f'Your current dice: {dice}\n')

			print('Your score sheet:')
			for category in player_scores:
				n = player_scores[category]
				if n == -1:
					n = '-'
				print(f'{category}: {n}')
			print()

			print('Select a category:')

			i = 1
			index_to_category_name = {}

			for category in player_scores:
				if player_scores[category] == -1:
					print(f'{i}. {category}')
					index_to_category_name[i] = category
					i += 1
			print()

			while True:
				try:
					selected_category = int(input('Category => '))
				except ValueError:
					print('Invalid category number.  Please try again!')
				else:
					if selected_category in index_to_category_name:
						break
					print('Invalid category number.  Please try again!')
			category_name = index_to_category_name[selected_category]

			player_scores[category_name] = calculate_score(category_name, dice)

	clear_screen()
	print('Game Over!\n')
	print('Leaderboard:')

	total_scores = []
	for player in names:
		pair = [
			calculate_total_score(scores[player]),
			player,
		]
		total_scores.append(pair)

	total_scores.sort(reverse=True)

	for pair in total_scores:
		print(f'{pair[1]}: {pair[0]}')

	print()
	input('Press any key to exit...')

main()
