from django.conf.urls import url
from django.conf.urls.static import static
from . import view,search,settings,find

static(settings.STATIC_URL, document_root=settings.STATIC_ROOT)
urlpatterns = [
               url(r'^index$', view.index),
               url(r'^$', view.index),
               url(r'^Search$', view.search),
               url(r'^Retrieval$', search.search),
               url(r'^home$', view.home),
               url(r'^Find$', find.find),
               url(r'^pdf$', view.pdf),
               ]
