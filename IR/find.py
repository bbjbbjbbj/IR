
from django.core.paginator import Paginator
from django.shortcuts import render
from . import query
# 全局变量，用于缓存查询语句，查询算法，查询结果
last_keyword = ""
last_algorithm = ""
last_query_result = {}

def find(request):
    global last_keyword, last_algorithm, last_query_result
    result = {}
    # 设置request编码方式
    request.encoding = 'utf-8'
    # 获取request参数
    keywords = request.GET['keywords']
    algorithm = request.GET['algorithm']
    page = int(request.GET['page'])
    # 查看缓存中是否有查询结果
    if keywords == last_keyword and last_algorithm == algorithm:
        result = last_query_result
    else:
        result = query.query(keywords, algorithm)
        last_keyword, last_algorithm, last_query_result = keywords, algorithm, result
    # 分页处理
    contact_list = result["content"]
    # 设置每页最大条数
    paginator = Paginator(contact_list, 5)
    # 分为4个子页面(1-5, 6-10, 11-15, 16-20)
    first = int((page - 1) / 5) * 5 + 1
    num = paginator.num_pages
    if not 0 < page < num+1:
        page = 1
    if((first + 4) > num):
        page_range = range(first, num + 1)
    else:
        page_range = range(first, first + 5)
    # 已做过边界检测, 直接访问页面
    contacts = paginator.page(page)
    #高亮
    keys = result['maybefinding']
    for content in contacts:
        # content['title'] = query.highlight(content['title'], keys)
        content['abstract'] = query.highlight(content['abstract'], keys)
    context = {
        'keywords': keywords,
        'al': algorithm,
        'time': result["time"],
        'count': result["count"],
        "maybefinding": result["maybefinding"],
        'contacts': contacts,
        'pagerange': page_range,
        'page': page,
        'num': num,
        "first": first
    }
    # 返还处理好的页面
    return render(request, 'ALR4.html', context)