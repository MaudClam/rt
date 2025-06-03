namespace rt {

[[noreturn]] inline void fail(const std::string& message, int code = 1) {
	logger.error(message);
	std::exit(code);
}

} // namespace rt
