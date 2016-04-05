SET(CORE_PRIVILEGE_DB ".core_privilege_info.db")
SET(WRT_PRIVILEGE_DB ".wrt_privilege_info.db")
SET(CORE_PRIVILEGE_MAPPING_DB ".core_privilege_mapping.db")
SET(WRT_PRIVILEGE_MAPPING_DB ".wrt_privilege_mapping.db")
SET(POLICY_DB ".policy.db")

IF("${PROFILE_TYPE}" STREQUAL "WEARABLE")
	SET(_profile "wearable")
ELSEIF("${PROFILE_TYPE}" STREQUAL "MOBILE")
	SET(_profile "mobile")
ELSEIF("${PROFILE_TYPE}" STREQUAL "TV")
	SET(_profile "tv")
ENDIF()

IF("${_profile}" STREQUAL "tv")
	ADD_CUSTOM_COMMAND(
		OUTPUT ${CORE_PRIVILEGE_DB} ${WRT_PRIVILEGE_DB} ${CORE_PRIVILEGE_MAPPING_DB} ${WRT_PRIVILEGE_MAPPING_DB} ${POLICY_DB}
		COMMAND ./tv_core_db_generator.sh && ./tv_wrt_db_generator.sh && ./tv_core_mapping_db_generator.sh && ./tv_wrt_mapping_db_generator.sh && ./policy_db_generator.sh
        )
ELSE()
	ADD_CUSTOM_COMMAND(
		OUTPUT ${CORE_PRIVILEGE_DB} ${WRT_PRIVILEGE_DB} ${CORE_PRIVILEGE_MAPPING_DB} ${WRT_PRIVILEGE_MAPPING_DB} ${POLICY_DB}
		COMMAND ./core_db_generator.sh ${_profile} && ./wrt_db_generator.sh ${_profile} && ./core_mapping_db_generator.sh ${_profile} && ./wrt_mapping_db_generator.sh ${_profile} && ./policy_db_generator.sh
		)
ENDIF()

ADD_CUSTOM_TARGET(privilege-db ALL DEPENDS ${CORE_PRIVILEGE_DB} ${WRT_PRIVILEGE_DB} ${CORE_PRIVILEGE_MAPPING_DB} ${WRT_PRIVILEGE_MAPPING_DB} ${POLICY_DB})

INSTALL(FILES ${CORE_PRIVILEGE_DB} ${WRT_PRIVILEGE_DB} ${CORE_PRIVILEGE_MAPPING_DB} ${WRT_PRIVILEGE_MAPPING_DB} ${POLICY_DB} ${POLICY_DB}-journal DESTINATION ${DATADIR}/privilege-manager/)

