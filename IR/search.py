# -*- coding: utf-8 -*-

from django.core.paginator import Paginator
from django.http import JsonResponse
import copy
import os
from . import query


# 全局变量，用于缓存查询语句，查询算法，查询结果
last_keywords = ""
last_algorithms = ""
last_query_results = {}


def search(request):
    global last_keywords, last_algorithms, last_query_results
    # 设置request编码方式, 获取参数
    request.encoding = 'utf-8'
    keywords = request.GET['keywords'].replace("%20", " ").replace("%22", "\"")
    algorithm = request.GET['algorithm']
    page = int(request.GET['page'])
    if not (keywords == last_keywords and algorithm == last_algorithms):
        last_query_results = query.query(keywords, algorithm)
        last_keywords = keywords
        last_algorithms = algorithm
    response = last_query_results.copy()
    response["content"] = response["content"][5 * (page - 1): 5 * page]
    response = copy.deepcopy(response)
    keys = response['maybefinding']
    count = 0
    for content in response['content']:
        count += 1
        content['title'] = query.highlight(content['title'], keys)[0]
        ab, flag = query.highlight(content['abstract'], keys)
        if flag == 0:
            ab, flag = query.highlight2(content['content'], keys)
        if flag == 0:
            if content['abstract'] != 'Unknown':
                ab = content['abstract']
            else:
                ab = '...'
        content['abstract'] = ab
        # print('id=',count,ab)
        del content['content']
    response["currentPage"] = str(page)
    return JsonResponse(response)
