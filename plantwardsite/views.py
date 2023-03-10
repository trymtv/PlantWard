import json
from django.shortcuts import render
from django.http import HttpResponse, JsonResponse
from django.views.decorators.csrf import csrf_exempt
from plantwardsite.models import Measurement, SensorDetails
from plantwardsite.mqtt import client


def index(request):
    return HttpResponse("Hello, world. You're at the polls index.")

@csrf_exempt
def threshold(request):
    if request.method == "POST":
        body = json.loads(request.body)
        rc, mid = client.publish("sensor/threshold", body['threshold'])
        return JsonResponse({"responseCode": rc})
    return HttpResponse(status=405)

@csrf_exempt
def sensor(request):
    if request.method == "POST":
        body = json.loads(request.body)
        measurement = Measurement(soil_hum=body["soil_hum"],
                                  air_hum=body["air_hum"], 
                                  air_temp=body["air_temp"])
        measurement.save()
        return HttpResponse(status=200)
    elif request.method == "GET":
        details = SensorDetails.objects.first()
        return HttpResponse(details.threshold)
    return HttpResponse(status=405)
