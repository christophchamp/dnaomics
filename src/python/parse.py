#!/usr/bin/python -B

def parse_reference(body, off):
	if body[off:].startswith('[['):
		off += 2
	else:
		return ((), off)

	end = body.find(']]', off)
	if end < 0:
		return ((), off-2)

	return body[off:end].split('|')


def parse_template(body, off):
	"""
		Parse the wikipedia {{...}} lists, used for wikiboxes
	"""

	if body[off:].startswith('{{'):
		off += 2
	else:
		return (None, off)

	i = off
	lst, items = [], []
	brackets = 0

	while True:
		c = body[i]

		if c == '|':
			if brackets:
				val = items.pop()
				items.append(val+c)
			else:
				lst.append(items)
				items = []
		elif c == '{' and body[i+1] == '{':
			itm, i = parse_template(body, i)
			items.append(itm)
			continue
		elif c == '}' and body[i+1] == '}':
			lst.append(items)
			break
		elif c == '[' and body[i+1] == '[':
			brackets = brackets + 1
			i = i + 1
		elif c == ']' and body[i+1] == ']':
			brackets = brackets - 1
			i = i + 1
		elif c in ('\n', '\t'):
			pass
		else:
			# If the last thing we added was a list, make another string
			if not items:
				items = ['']
			if not isinstance(items[-1], basestring):
				items.append(u'')
			items[-1] = items[-1] + c

		i = i + 1

	ret = []
	for elm in lst:
		if isinstance(elm, list) and len(elm) == 1:
			ret.append(elm[0])
		else:
			ret.append(elm)

	return ret, i+2

