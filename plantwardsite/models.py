from django.db import models

#defines the database table 

class Measurement(models.Model):
    soil_hum = models.FloatField()
    air_hum = models.FloatField()
    air_temp = models.FloatField()
    timestamp = models.DateTimeField(auto_now_add=True)

class SensorDetails(models.Model):
    threshold = models.IntegerField()