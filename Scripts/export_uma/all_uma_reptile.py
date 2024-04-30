import requests
from bs4 import BeautifulSoup
import re
import json

# 读取JSON文件
with open('example.json', 'r') as file:
    example_json = json.load(file)

# 从文件中读取文本
with open('links.txt', 'r', encoding='utf-8') as file:
    text = file.read()


# 使用正则表达式提取名称
names = re.findall(r'/umamusume/characters/(\d+-[a-z-]+)', text)
#print(names)
urls=["https://gametora.com/umamusume/characters/"+i for i in names]

pool = {}

for URL in urls:
    nameid=URL[42:]
    umaid=int(nameid[:6])
    name=nameid[7:]


    umadata = json.loads(json.dumps(example_json))
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/91.0.4472.124 Safari/537.36"
    }

    response = requests.get(URL, headers=headers)
    soup = BeautifulSoup(response.content, 'html.parser')
    #print(soup)
    t=soup.find_all('span', class_='utils_stat_icon__iOuJl')

    statusNames=["Speed","Stamina","Power","Guts","Wisdom"]

    for statusId in range(5):
        c=0

        for x in t:
            t1=x.parent
            t2=t1.find('img', alt=statusNames[statusId])
            if t2:
                number_div = t2.parent.parent.find_next_sibling('div')
                if number_div:
                    #print(number_div.text)
                    c+=1
                    if(c==1):#三星马
                        pass
                    elif(c==2):#五星马
                        umadata["fiveStatusInitial"][statusId]=int(number_div.text)
                    elif(c==3):#属性加成
                        bonus=-100
                        t3=number_div.text
                        if(t3=='-'):
                            bonus=0
                        else:
                            if t3.endswith('%'):
                                bonus=int(t3[:-1])
                            else:
                                print("error:",t3)
                        umadata["fiveStatusBonus"][statusId]=bonus



    t=soup.find_all('div', class_="characters_objective_text__VbDAs")
    all_races=[]
    free_races = []
    last_goal = 0
    for t1 in t:
        t2=t1.find_all('div')
        #print(t2)
        if(len(t2)!=4 or ("Turf" not in t2[3].text and "Dirt" not in t2[3].text)): #非单场比赛目标
            print(nameid,t2[0].text)
            end_turn = int(t2[1].text.split(" ")[1])
            if "fans" in t2[0].text:    # 粉丝数要求
                free_races.append(dict(startTurn=last_goal, endTurn=end_turn-1, count=1))
                print(f"Turn {last_goal+1} -> {end_turn}, 1 free race")
            elif "races" in t2[0].text: # 场次要求
                count = int(t2[0].text.split(" in ")[1].split(" ")[0])
                free_races.append(dict(startTurn=last_goal, endTurn=end_turn-1, count=count))
                print(f"Turn {last_goal+1} -> {end_turn}, {count} free race(s)")
            last_goal = end_turn+1
        else:
            last_goal = int(t2[1].text[5:7])
            all_races.append(last_goal-1)        

    umadata["gameId"]=umaid
    umadata["name"]=name
    umadata["races"]=all_races
    umadata["freeRaces"]=free_races
    pool[umaid] = umadata

#    with open('uma/'+nameid+".json", 'w') as file:
#        json.dump(umadata, file, indent=4)

sortedPool = dict(sorted(pool.items(), key=lambda x: int(x[0])))

with open('umaDB.json', 'w') as file:
    json.dump(sortedPool, file, indent=2)


        #print(URL,umadata["fiveStatusBonus"])
        # Extracting character URLs
