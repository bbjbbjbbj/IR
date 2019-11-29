from django.http import HttpResponse
from django.shortcuts import render
def index(request):
    return render(request, 'ALR1.html')
def search(request):
    return render(request, 'ALR2.html')
def home(request):
    return render(request, 'ALR3.html')
def pdf(request):
    pdfId = request.GET['id']
    # pdf_data=open('pdf/' + pdfId + '.pdf','rb').read()
    pdf_data=open('pdf/test.pdf','rb').read()
    return HttpResponse(pdf_data, content_type='application/pdf')
