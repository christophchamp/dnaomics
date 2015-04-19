#!/usr/bin/python -B

import re
import datetime
import dateutil
import parse
import template

_human_infoboxes = ('person', 'person/sandbox', 'actor', 'adult female',
  'adult male', 'american indian chief', 'antipopestyles', 'architect',
  'art group', 'artist', 'artist/sandbox', 'astronaut', 'aviator',
  'bishopbiog', 'bishopstyles', 'cardinal', 'cardinalbiog', 'cardinalstyles',
  'chef', 'chef/sandbox', 'chess player', 'chinese-language singer and actor',
  'christian leader', 'christian leader/sandbox', 'christian biography',
  'clergy', 'college coach', 'comedian', 'comedian/sandbox', 'comics creator',
  'coptic popestyles', 'criminal', 'dancer', 'economist', 'engineer',
  'fashion designer', 'french parliamentarian', 'fbi ten most wanted',
  'go player', 'horseracing personality', 'imam', 'internet celebrity',
  'journalist', 'journalist/sandbox', 'linguist', 'member of the knesset',
  'mmastats', 'martial artist/old', 'martyrs', 'mass murderer',
  'medical person', 'military person', 'military person/sandbox', 'model',
  'musical artist', 'musical artist/sandbox', 'nhl coach', 'officeholder',
  'officeholder/sandbox', 'ottoman sultan', 'patriarchstyles', 'pharaoh',
  'philosopher', 'pirate', 'pirate/sandbox', 'playboy playmate',
  'playboy playmate/sandbox', 'poker player', 'police officer',
  'polish politician', 'pope', 'popestyles', 'presenter', 'rabbi',
  'religious biography', 'revolution biography', 'saint', 'saint/sandbox',
  'scientist', 'scientist/sandbox', 'serial killer', 'spy', 'sumo wrestler',
  'tennis biography', 'paranormalpeople1', 'university chancellor', 'vandal',
  'vandal/sandbox', 'vice-regal', 'writer', 'writer/sandbox',
  'television personality',
    # manually added
    'minister','monarch',)

# TODO:  'musical artist'  can also mean band

def extract_persondata(body):

    m = re.compile('{{persondata').search(body)

    if not m:
        return

    start, end = m.span()
    tmp, end = template.parse_template(body, start)

    persondata = template.Infobox(tmp)

    #person = template.Person()
    #person.populate_from_persondata(persondata)

    return persondata

def extract_infobox(body):
    """
    Extract infobox from body, returning dictionary.
    If doesn't exist, return None
    """
    ib = re.compile('{{(infobox[ _]([^<\n|}]*))\s*')
    global _human_infoboxes

    m = ib.search(body)
    if not m:
        return None, 0

    if not m.group(2).strip().lower() in _human_infoboxes:
        return None, 0

    # We found it, parse it
    start, finish = m.span()
    infobox, end = template.parse_template(body, start)
    if infobox:
        person = template.get_template(infobox)
        return (person, end)

    # Convert into dictionary
    ret = {}
    ret['type'] = lst.pop(0)
    ret['misc'] = []

    for elm in lst:
        if not elm:
            continue
        if isinstance(elm, list):
            k, v = elm[0].split('=', 1)
            if v.strip():
                v = [v]
            else:
                v = []
            v = v + elm[1:]
        else:
            try:
                k, v = elm.split('=', 1)
                v = v.strip().split('<br>')[0].split('<br />')[0].split('<br/>')
            except ValueError:
                ret['misc'].append(elm)
                continue
        ret[k.strip().lower().encode('ascii', 'ignore')] = v

    def _wikidate_to_date(d):
        # Probably a string like '900 BC' or '988 AD'
        if not isinstance(d, list):
            return None # for now

        # TODO: clean this mess up
        d = filter(lambda x: isinstance(x, basestring),d)
        if d[0].lower() == 'death date and age':
            d = d[:-3]
        if d[0].lower() == 'birth date and age':
            d = filter(lambda x: x.isdigit(), d)
            d = d[-3:]
        elif d[0].lower() == 'bday':
            return dateutil.parser.parse(d[1])
        elif d[0].lower() == 'birth-date':
            return dateutil.parser.parse(d[-1])
        d = filter(lambda x: x.isdigit(), d)
        d = map(lambda x: int(x), d)
        return datetime.date(*d)

    # format birth/death dates
    if ret.get('birthdate') and ret['birthdate'][0]:
        ret['birthdate'] = _wikidate_to_date(ret['birthdate'][0])
    if ret.get('birth_date') and ret['birth_date'][0]:
        if isinstance(ret['birth_date'][0],list):
            ret['birth_date'][0].pop(0)
        ret['birth_date'] = _wikidate_to_date(ret['birth_date'][0])

    if ret.get('deathdate') and ret['deathdate'][0]:
        ret['deathdate'] = ret['deathdate'][0]
        ret['deathdate'] = _wikidate_to_date(ret['deathdate'][0])
    if ret.get('death_date') and ret['death_date'][0]:
        ret['death_date'] = ret['death_date'][0]
        ret['death_date'] = _wikidate_to_date(ret['death_date'][0])
        

    return ret, end

if __name__ == '__main__':
    import pprint
    p = pprint.PrettyPrinter(indent=4)
    p.pprint(extract_infobox('''{{Infobox officeholder
|name = Abraham Lincoln
|nationality = American
|image = Abraham Lincoln head on shoulders photo portrait.jpg
|order = [[List of Presidents of the United States|16th]] [[President of the United States]]
|term_start = March 4, 1861
|term_end = April 15, 1865
|predecessor = [[James Buchanan]]
|successor = [[Andrew Johnson]]
|state2 = [[Illinois]]
|district2 = [[Illinois's 7th congressional district|7th]]
|term_start2 = March 4, 1847
|term_end2 = March 3, 1849
|predecessor2 = [[John Henry (representative)|John Henry]]
|successor2 = [[Thomas L. Harris]]
|birth_date = {{birth date|mf=yes|1809|2|12|mf=y}}
|birth_place =[[Hardin County, Kentucky]]
|death_date = {{death date and age|mf=yes|1865|4|15|1809|2|12}}
|death_place =[[Washington, D.C.]]
|restingplace=[[Oak Ridge Cemetery]]<br/>[[Springfield, Illinois|Springfield]], [[Illinois]]<br />{{Coord|39|49|24|N|89|39|21|W|type:landmark}}
|spouse = [[Mary Todd Lincoln]]
|children = [[Robert Todd Lincoln]], [[Edward Baker Lincoln|Edward Lincoln]], [[William Wallace Lincoln|Willie Lincoln]], [[Tad Lincoln]]
|occupation=[[Lawyer]]
|religion = See: [[Abraham Lincoln and religion]]
|signature = Abraham Lincoln Signature.svg
|branch=Illinois Militia
|serviceyears=1832
|battles=[[Black Hawk War]]
}}'''))

