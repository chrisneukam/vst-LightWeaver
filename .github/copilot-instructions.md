Always follow C++20 standards and JUCE 8 best practices.
Refer to docs/PROJECT_CONTEXT.md for project architecture and parameter naming.
Never allocate memory or use locks inside AudioProcessor::processBlock().
Keep UI components strictly inside Source/UI/ and separate from AudioProcessor.