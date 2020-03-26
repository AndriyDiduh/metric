import numpy
from metric.distance import SSIM


img1 = numpy.loadtxt('test_data/img1.csv')
img2 = numpy.loadtxt('test_data/img1.csv')


def test_distance():
    distance = SSIM()
    assert distance(img1, img2) == 0.09074577584588275
    assert distance(img2, img1) == 0.09074577584588275


def test_config_defaults():
    distance = SSIM()

    assert distance.dynamic_range == 255
    assert distance.masking == 2


def test_config_override():
    distance = SSIM(dynamic_range=1, masking=20)
    assert distance.dynamic_range == 1
    assert distance.masking == 20
