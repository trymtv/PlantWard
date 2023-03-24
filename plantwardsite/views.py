import json
import random
from django.shortcuts import render
from django.http import HttpResponse, JsonResponse, HttpResponseRedirect
from django.views.decorators.csrf import csrf_exempt
from plantwardsite.models import Measurement, SensorDetails
from plantwardsite.mqtt import client
from django.forms.models import model_to_dict

def create_dummies():
    last_measurement = Measurement.objects.last()
    dummies = [last_measurement]
    for i in range(5):
        dummy = Measurement()
        dummy.soil_hum = random.randrange(20, 50)
        dummy.air_hum = random.randrange(20, 50)
        dummy.air_temp = random.randrange(10, 30)
        dummies.append(dummy)
    return dummies


def index(request):
    last_measurement = Measurement.objects.last()
    threshold = SensorDetails.objects.last()
    rand_num = random.randrange(-2, 3)
    dummies = create_dummies()
    return render(request, 'index.html', {"sensor_values": last_measurement, "threshold":threshold, 
    "range": range(1,4), "rand_num": rand_num, "dummies": dummies})

@csrf_exempt
def threshold(request):
    if request.method == "POST":
        threshold = request.POST["threshold"]
        rc, mid = client.publish("sensor/threshold", threshold)
        details = SensorDetails.objects.last()
        details.threshold = threshold
        details.save()
        return HttpResponseRedirect("/")
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
        details = SensorDetails.objects.last()
        sensor_measurement = Measurement.objects.last()
        return JsonResponse({**model_to_dict(details), **model_to_dict(sensor_measurement)})
    return HttpResponse(status=405)
