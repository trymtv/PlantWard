from django.urls import path

from . import views

urlpatterns = [
    path('', views.index, name='index'),
    path('sensor', views.sensor, name='sensor'),
    path('threshold', views.threshold, name='threshold'),
]