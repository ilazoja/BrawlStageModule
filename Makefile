#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

all: st_final.rel st_village.rel st_otrain.rel st_starfox.rel st_stadium.rel st_boss.rel st_bowsercastle.rel st_cheesebridge.rel st_finalsub.rel st_finkrat.rel st_ghosthouse.rel st_gimmick.rel st_gyromite.rel st_izumi.rel st_kingofthehill.rel st_kongojungleredux.rel st_krazoa.rel st_lava.rel st_lunarhorizon.rel st_phendrana.rel st_pictsimple.rel st_pointlessexpress.rel st_qbert.rel st_skypillar.rel st_skytower.rel st_smashketball.rel st_soup.rel st_templeoftime.rel st_warfactory.rel st_worldtournament.rel demos

demos: st_demo.rel st_camera.rel st_effect.rel

st_final.rel:
	@cd st_final && make clean && make

st_demo.rel:
	@cd demos/st_demo && make clean && make

st_village.rel:
	@cd st_village && make clean && make

st_otrain.rel:
	@cd st_otrain && make clean && make

st_starfox.rel:
	@cd st_starfox && make clean && make

st_stadium.rel:
	@cd st_stadium && make clean && make

st_camera.rel:
	@cd demos/st_camera && make clean && make

st_effect.rel:
	@cd demos/st_effect && make clean && make

st_boss.rel:
	@cd custom/st_boss && make clean && make

st_bowsercastle.rel:
	@cd custom/st_bowsercastle && make clean && make

st_cheesebridge.rel:
	@cd custom/st_cheesebridge && make clean && make

st_finalsub.rel:
	@cd custom/st_finalsub && make clean && make

st_finkrat.rel:
	@cd custom/st_finkrat && make clean && make

st_ghosthouse.rel:
	@cd custom/st_ghosthouse && make clean && make

st_gimmick.rel:
	@cd custom/st_gimmick && make clean && make

st_gyromite.rel:
	@cd custom/st_gyromite && make clean && make

st_izumi.rel:
	@cd custom/st_izumi && make clean && make

st_kingofthehill.rel:
	@cd custom/st_kingofthehill && make clean && make

st_kongojungleredux.rel:
	@cd custom/st_kongojungleredux && make clean && make

st_krazoa.rel:
	@cd custom/st_krazoa && make clean && make

st_lava.rel:
	@cd custom/st_lava && make clean && make

st_lunarhorizon.rel:
	@cd custom/st_lunarhorizon && make clean && make

st_phendrana.rel:
	@cd custom/st_phendrana && make clean && make

st_pictsimple.rel:
	@cd custom/st_pictsimple && make clean && make

st_pointlessexpress.rel:
	@cd custom/st_pointlessexpress && make clean && make

st_qbert.rel:
	@cd custom/st_qbert && make clean && make

st_skypillar.rel:
	@cd custom/st_skypillar && make clean && make

st_skytower.rel:
	@cd custom/st_skytower && make clean && make

st_smashketball.rel:
	@cd custom/st_smashketball && make clean && make

st_soup.rel:
	@cd custom/st_soup && make clean && make

st_templeoftime.rel:
	@cd custom/st_templeoftime && make clean && make

st_warfactory.rel:
	@cd custom/st_warfactory && make clean && make

st_worldtournament.rel:
	@cd custom/st_worldtournament && make clean && make

clean:
	@cd st_final && make clean
	@cd st_village && make clean
	@cd demos/st_demo && make clean
	@cd st_otrain && make clean
	@cd st_starfox && make clean
	@cd st_stadium && make clean
	@cd demos/st_camera && make clean
	@cd demos/st_effect && make clean
	@cd custom/st_boss && make clean
	@cd custom/st_bowsercastle && make clean
	@cd custom/st_cheesebridge && make clean
	@cd custom/st_finalsub && make clean
	@cd custom/st_finkrat && make clean
	@cd custom/st_ghosthouse && make clean
	@cd custom/st_gimmick && make clean
	@cd custom/st_gyromite && make clean
	@cd custom/st_izumi && make clean
	@cd custom/st_kingofthehill && make clean
	@cd custom/st_kongojungleredux && make clean
	@cd custom/st_krazoa && make clean
	@cd custom/st_lava && make clean
	@cd custom/st_lunarhorizon && make clean
	@cd custom/st_phendrana && make clean
	@cd custom/st_pictsimple && make clean
	@cd custom/st_pointlessexpress && make clean
	@cd custom/st_qbert && make clean
	@cd custom/st_skypillar && make clean
	@cd custom/st_skytower && make clean
	@cd custom/st_smashketball && make clean
	@cd custom/st_soup && make clean
	@cd custom/st_templeoftime && make clean
	@cd custom/st_warfactory && make clean
	@cd custom/st_worldtournament && make clean