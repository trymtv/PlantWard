from django.contrib import admin

from .models import Measurement, SensorDetails


class MeasurementAdmin(admin.ModelAdmin):
    readonly_fields = ('timestamp',)

admin.site.register(Measurement, MeasurementAdmin)
admin.site.register(SensorDetails)
