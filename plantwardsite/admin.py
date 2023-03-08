from django.contrib import admin

from .models import Measurement

class MeasurementAdmin(admin.ModelAdmin):
    readonly_fields = ('timestamp',)

admin.site.register(Measurement, MeasurementAdmin)
