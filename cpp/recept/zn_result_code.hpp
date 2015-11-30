#pragma once

enum zn_result_code
{
	RC_ACCEPT			= 201,
	RC_EXIST			= 202,
	RC_FORMAT_ERR		= 401,
	RC_SERV_BUSY		= 501,
	RC_EAS_BUSY		= 502,
	RC_SERV_MAINT		= 511,
	RC_EAS_MAINT		= 512,
	RC_DB_ERR			= 521,
	RC_SERV_ERR		= 522
};
