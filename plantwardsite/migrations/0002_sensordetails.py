# Generated by Django 4.1.7 on 2023-03-08 10:57

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('plantwardsite', '0001_initial'),
    ]

    operations = [
        migrations.CreateModel(
            name='SensorDetails',
            fields=[
                ('id', models.BigAutoField(auto_created=True, primary_key=True, serialize=False, verbose_name='ID')),
                ('threshold', models.IntegerField()),
            ],
        ),
    ]