from ctypes import *
import nltk.stem.porter as pt
import nltk.tokenize as tk
import time
import pickle
import os
import re
os.chdir('IR')
server = cdll.LoadLibrary("./main.so")
server.start()
total_obj = pickle.load(open('pkl/total2.pkl', 'rb'))
MAX_DOCUMENT_ID = 1000
MAX_LENGTH = 1000


def is_english_word(str):
    for s in str:
        if s < 'A' or 'Z' < s < 'a' or 'z' < s:
            return 0
    return 1


def porter(word):
    pt_stemmer = pt.PorterStemmer()  # porter词干提取器
    return pt_stemmer.stem(word)


def highlight(doc, keys):
    flag = 0
    highlight_start = '<em>'
    highlight_end = '</em>'
    start_offset = 0
    MAX_ITEM = 160
    origin_word_list = tk.word_tokenize(doc)
    stem_word_list = [porter(origin_word.lower()) for origin_word in origin_word_list]
    for i in range(len(origin_word_list)):
        if stem_word_list[i] in keys:
            origin_word_list[i] = highlight_start + origin_word_list[i] + highlight_end
            if flag == 0:
                flag = 1
                start_offset = i
    if start_offset < MAX_ITEM / 2:
        return ' '.join(origin_word_list), flag
    else:
        return '... ' + ' '.join(origin_word_list)[start_offset - 50:], flag


def highlight2(doc, keys):
    highlight_start = '<em>'
    highlight_end = '</em>'
    start_offset = 0
    MAX_ITEM = 1160
    origin_word_list = tk.word_tokenize(doc)
    stem_word_list = [porter(origin_word.lower()) for origin_word in origin_word_list]
    for i in range(len(origin_word_list)):
        if stem_word_list[i] in keys:
            start_offset = i
            break
    if start_offset == 0:
        return 'Unknown', 0
    origin_word_list = origin_word_list[start_offset: start_offset + MAX_ITEM]
    stem_word_list = stem_word_list[start_offset: start_offset + MAX_ITEM]
    for i in range(len(origin_word_list)):
        if stem_word_list[i] in keys:
            origin_word_list[i] = highlight_start + origin_word_list[i] + highlight_end
    return '... ' + ' '.join(origin_word_list), 1


def query(keywords, algorithm):
    # 单词拆分
    print("para:", keywords, algorithm)
    query_list = keywords.split(" ")
    # 进行拼写校验
    normal, other = [], []
    for query in query_list:
        if is_english_word(query):
            normal.append(porter(query))
        elif '*' in query:
            if not is_english_word(query.replace("*", "a")):
                return {"status": "error"}
            else:
                other.append(query)
        elif "\"" == query[0] and "\"" == query[-1]:
            if not is_english_word(query[1:-1]):
                return {"status": "error"}
            else:
                other.append(query[1:-1])
        else:
            return {"status": "error"}
    # 装配并调用查询函数
    normal_query, other_query, maybe_query = (
        create_string_buffer(MAX_LENGTH) for i in range(3))
    document = c_int * MAX_DOCUMENT_ID
    document = document()
    score = c_float * MAX_DOCUMENT_ID
    score = score()
    count, start_time, end_time = 0, 0, 0
    if algorithm == "Fulltext":
        if len(normal) < 1 or len(other) > 0:
            return {"status": "error"}
        normal_query = bytes(" ".join(normal), encoding='ascii')
        start_time = time.time()
        count = server.query(normal_query, other_query,
                             maybe_query, document, score, 1)
        end_time = time.time()
    elif algorithm == "Wildcard":
        if len(other) < 1:
            return {"status": "error"}
        normal_query = bytes(" ".join(normal), encoding='ascii')
        other_query = bytes(" ".join(other), encoding='ascii')
        start_time = time.time()
        count = server.query(normal_query, other_query,
                             maybe_query, document, score, 2)
        end_time = time.time()
    elif algorithm == "Correction":
        if len(normal) < 1:
            return {"status": "error"}
        for o in other:
            if not is_english_word(o):
                return {"status": "error"}
        normal_query = bytes(" ".join(other), encoding='ascii')
        other_query = bytes(" ".join(normal), encoding='ascii')
        start_time = time.time()
        count = server.query(normal_query, other_query,
                             maybe_query, document, score, 3)
        end_time = time.time()
    elif algorithm == "Phrase":
        if len(normal) < 1 or len(other) > 0:
            return {"status": "error"}
        normal_query = bytes(" ".join(normal), encoding='ascii')
        start_time = time.time()
        count = server.query(normal_query, other_query,
                             maybe_query, document, score, 4)
        end_time = time.time()
    else:
        return {"status": "error"}
    result = {"status": "right", "count": count, "time": end_time - start_time}
    result["maybefinding"] = maybe_query.raw.strip(b'\x00').decode("ascii").split(' ')
    if algorithm == 'Fulltext' or algorithm == 'Phrase':
        result["maybefinding"] = normal
    result["algorithm"] = algorithm
    result["content"] = []
    content_moudle = {
        "title": "Paper1",
        "url": "http://www.baicu.com",
        "score": "0.53",
        "author": "Yuchen Lu, Jie Bao",
        "publishDate": "Unknown",
        "pulbPage": "Unknown",
        "abstract": "Early <em style=\"color:red\">classification</em> of time series is the prediction of the class label of a time series before it is observed in its entirety. In time-sensitive domains where information is collected over time it is worth sacrificing some classification accuracy in favor of earlier predictions"
    }
    for i in range(count):
        obj = total_obj[document[i]]
        content = obj.copy()
        content["url"] = "http://localhost:8000/pdf?id=" + str(document[i])
        content["score"] = str(score[i])[:4]
        content["author"] = ", ".join(content["author"])
        result["content"].append(content)
    return result
